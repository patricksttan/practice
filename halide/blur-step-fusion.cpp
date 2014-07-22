#include <Halide.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <sys/time.h>

namespace{
  double wtime() {
    timeval tv;
    gettimeofday(&tv,NULL);
    return double(tv.tv_sec) + double(tv.tv_usec)*1e-6;
  }
}

const int N_FUSION = 4;

double bench(int NX, int NY, int MAX_T) {

  Halide::Var x("x"),y("y"), yo("yo"), yi("yi"), xo("xo"), xi("xi");
  Halide::Func initial_condition("initial_condition");
  initial_condition(x, y) = 0.0f;
  initial_condition(NX/3, NY/4) = 1.0f* NX*NY ;
 
  Halide::ImageParam inPar(Halide::Float(32), 2, "inPar"); // 32bit float 2D 

  Halide::Image<float_t> input = initial_condition.realize(NX,NY); 
  Halide::Image<float_t> output(NX,NY); 
  

  // precompile the programs
  const float a = 0.5f, b = 0.25f;
  //std::vector<Halide::Func> cell2(N_FUSION), cell3(N_FUSION);
  Halide::Func cell2[N_FUSION], cell3[N_FUSION];

  for(int i_f = 0; i_f < N_FUSION ; ++i_f) {
    std::cerr << "define cell2-" << i_f << std::endl;
    if (i_f==0) {
      cell2[i_f](x,y)= (a * inPar(x,y) + b * inPar(clamp(x+1,0,NX-1),y) + b * inPar(clamp(x-1,0,NX-1),y)) ;
    } else {
      cell2[i_f](x,y)= (a * cell3[i_f-1](x,y) + b *  cell3[i_f-1](clamp(x+1,0,NX-1),y) + b * cell3[i_f-1](clamp(x-1,0,NX-1),y)) ;
    }

    std::cerr << "define cell3-" << i_f << std::endl;    
    cell3[i_f](x,y)= (a * cell2[i_f](x,y) + b * cell2[i_f](x,clamp(y+1,0,NY-1)) + b * cell2[i_f](x,clamp(y-1,0,NY-1))) ;

  }


  Halide::Var nid("nid");
  for(int i_f = N_FUSION-1; i_f >=0 ; --i_f) {
    if (i_f==N_FUSION-1){
      cell3[i_f].tile(x,y, xo,yo, xi, yi, NX/32,NY/32).fuse(xo,yo,nid).parallel(nid).vectorize(xi,8).unroll(xi,4);
    }
    else
      cell3[i_f].store_at(cell3[N_FUSION-1],nid).compute_at(cell3[N_FUSION-1], yi).vectorize(x,8).unroll(x,4);
    cell2[i_f].store_at(cell3[N_FUSION-1], nid).compute_at(cell3[N_FUSION-1], yi).vectorize(x,8).unroll(x,4);
  }


  std::cerr << "all strategy set"<< std::endl;    
  

  // run the computation once to generate the program
  inPar.set(input);
  output=cell3[N_FUSION-1].realize(NX,NY);
  std::swap(input, output);
  {
    std::vector<Halide::Argument> arg_vect;
    arg_vect.push_back(Halide::Argument("inPar", true, Halide::Int(32)));
    cell3[N_FUSION-1].compile_to_assembly("blur-fusion-gen.s", arg_vect, "blur");
  }


  double t0 = wtime();
  
  for (int t=0; t<=MAX_T; ++t) {
    // updating logic
    inPar.set(input);
    output=cell3[N_FUSION-1].realize(NX,NY);
    // swap the double-buffer
    std::swap(input, output);
  }

  double t1 = wtime();
  
  // output the final state
  {
    std::ofstream ofs("debug.txt");
    float ret = 0;
    for (int j = 0; j < NY; j++) {
      for (int i = 0; i < NX; i++) {
        ret += output(i, j);
      }
    }
    ofs << ret << std::endl;
    ofs.close();
  } 

  return t1-t0;
}



int main(int argc, char **argv) {
  using std::cout;
  using std::cerr;
  using std::endl;
  // uisng std::endl;


  for (size_t t_max=256; ; t_max*=2) {
    size_t ny = 2<<10;
    size_t nx = 2<<10;
    double deltaT = bench(nx,ny,t_max/N_FUSION);
    double num_flop =  double(nx) * double(ny) * double(t_max) * 10 ;
    cout << t_max << " " << deltaT << endl;
    cout << (num_flop / deltaT/1e9) <<  " GFlops" << endl;
  }


//  for (size_t ny = 16; ny < (2<<15); ny*=2) {
//    for (size_t nx = 16; nx < (2<<15); nx*=2) {
//      if (nx*ny >= (2<<28)) continue;
//
//      for (size_t t_max = 1; t_max <= 1000;t_max *= 10) {
//	
//	double t0 = wtime();
//	bench(nx,ny,t_max);
//	double t1 = wtime();
//	cout << nx << " " << ny << " " << t_max << " " << (t1-t0) << std::endl;
//      }
//    }
//  }
}
