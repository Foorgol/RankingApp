#!/bin/bash

rm array_view.h fail_fast.h gsl.h string_view.h

for f in array_view.h fail_fast.h gsl.h string_view.h; do
  wget https://raw.githubusercontent.com/Microsoft/GSL/master/include/$f
done

