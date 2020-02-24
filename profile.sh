#!/bin/sh

N=1000

start=$(date +%s%N)
for i in `seq 1 $N`; do
    ./bin/pj1 bin/pj1 --part=1.1 --input=10101010101010101010101010101010 --quiet > /dev/null
done
end=$(date +%s%N)

echo "Part 1.1: $[(end-start)/$N]ns/iteration"

start=$(date +%s%N)
for i in `seq 1 $N`; do
    ./bin/pj1 bin/pj1 --part=1.2 --input=10101010101010101010101010101010 --quiet > /dev/null
done
end=$(date +%s%N)

echo "Part 1.2: $[(end-start)/$N]ns/iteration"

start=$(date +%s%N)
for i in `seq 1 $N`; do
    ./bin/pj1 bin/pj1 --part=2 --input=10101010101010101010101010101010 --generator=0101010101010101 --quiet
done
end=$(date +%s%N)

echo "Part 2: $[(end-start)/$N]ns/iteration"
