set terminal pdf size 2, 1 font "Helvetica,8"
set output 'task1.pdf'

set xlabel 'epochs'
set ylabel 'accuracy'
set yrange [0:]
plot "task1_method1.txt" using 7 with lines title 'W depends on D',\
plot "task1_method1.txt" using 7 with lines title 'W independent of D',\

