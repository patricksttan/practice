{-# LANGUAGE FlexibleContexts #-}
{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE FunctionalDependencies #-}
{-# LANGUAGE MultiParamTypeClasses #-}
{-# LANGUAGE UndecidableInstances #-}

import qualified Data.Vector as V

-- xS and yS are type level, first-in first-out list.


-- | the type-class states that if you insert 
--   (v a) into vxS, the resulting type is vyS
class Insert v a vxS vyS | v a vxS -> vyS where
  insert :: v a -> vxS -> vyS

instance Insert f a () (f a,()) where
  insert va () = (va, ())

instance  (Insert f a vxS vyS) => Insert f a (vx,vxS) (vx,vyS) where
  insert va (vb,vbS) = (vb, insert va vbS)

vi1 :: V.Vector Int
vi1 = V.fromList [100..102]

vc1 :: V.Vector Char
vc1 = V.fromList ['a'..'c']

vd1 :: V.Vector Double
vd1 = V.fromList [1.1, 1.4, 1.9]


-- | apply the type level, fifo list to a function.
class Apply v a b vxS r | b vxS -> r where
  apply :: (a -> b) -> (v a, vxS) -> v r 

instance (Functor v) => Apply v a b () b where
  apply a2b (va, ())= fmap a2b va



main = do
  print $ insert vi1 $ insert vc1 $ insert vd1 ()
  print $ apply ((*2)) (vi1, ())
