set terminal pdf size 4, 2 font "Helvetica,8"
set output 'task1.pdf'

set xlabel 'epochs'
set ylabel 'accuracy'
set yrange [0:]
plot "task1_method1.txt" using 2 with lines title 'W depends on D',\
     "task1_method2.txt" using 2 with lines title 'W independent of D',\

