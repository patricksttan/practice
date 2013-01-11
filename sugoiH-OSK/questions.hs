-- すごいHaskell読書会 in 大阪 #3

-- 練習問題



-- 問題1

-- 次のリスト内包を **map** と **filter** で書き直してみましょう

-- [ x ^ 2 | x <- [1..5], odd x]



-- 問題2

-- 標準関数 **takeWhile'** と **dropWhile'** を実装してみましょう。



-- takeWhile 関数

-- リストの先頭から述語を満たす連続した要素を取り出します。

-- GHCi > taleWhile' (< 5) [1..10]

-- [1,2,3,4]

takeWhile' :: (a -> Bool) -> [a] -> [a]



-- dropWhile 関数

-- リストの先頭から述語を満たす連続した要素をのぞいた残りを返します。

-- GHCi> dropWhile' (< 5) [1..10]

-- [5,6,7,8,9,10]

dropWhile' :: (a -> Bool) -> [a] -> [a]



-- 問題3

-- 標準関数 any と all を畳み込みを使って実装してみましょう。



-- any は述語関数とリストをとって、

-- 要素がひとつでも述語を満たすときTrueを返します。

any' :: (a -> Bool) -> [a] -> Bool



-- all は述語関数とリストをとって、

-- すべての要素が述語を満たすときTrueを返します。

all' :: (a -> Bool) -> [a] -> Bool



-- 問題4

-- ここに、こんな２項演算子のリストがあります。

--

-- [(-), (div), (^)]

--

-- このリストの中の演算子の左辺に2を、 右辺に3を

-- 適用した結果のリストを得るには、map関数と

-- ラムダ式を使ったこんな方法が考えられます。

--

-- map (\f -> 2 `f` 3) [(-), (div), (^)]

--

-- さて、map関数を使うという方針はそのままに、

-- ($)と(.)を使ってこの式からラムダ式を消し去りましょう。



-- 問題5

-- Project Euler 14問目「最長のコラッツ数」にチャレンジしてみましょう。

-- http://projecteuler.net/problem=14

-- 日本語訳

-- http://odz.sakura.ne.jp/projecteuler/index.php?cmd=read&page=Problem%2014
