set terminal pdf size 2, 2 font "Helvetica,8"
set output 'task0-cluster-alpha-1.pdf'
stats 'task0_method1-alpha-1-output.txt' nooutput
last_index = int(STATS_records - 1)
set key top left
set key at -1.7,1.4
set xlabel 'x'
set ylabel 'y'
plot "task0_method1-alpha-1-input.txt" using 1:2 pointtype 0 ps 1 title 'data',\
     "task0_method1-alpha-1-output.txt" using 1:2 with lines lc rgb "blue" lw 3 notitle '', '' using 1:2 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" notitle,\
     "task0_method1-alpha-1-output.txt" using 3:4 with lines lc rgb "blue" lw 3 notitle, '' using 3:4 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" notitle, \
     "task0_method1-alpha-1-output.txt" using 5:6 with lines lc rgb "blue" lw 3 title "center paths", '' using 5:6 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" title 'final centers'

set terminal pdf size 2, 2 font "Helvetica,8"
set output 'task0-cluster-alpha-10.pdf'
stats 'task0_method1-alpha-10-output.txt' nooutput
last_index = int(STATS_records - 1)
set key top left
set key at -1.7,1.4
set xlabel 'x'
set ylabel 'y'
plot "task0_method1-alpha-10-input.txt" using 1:2 pointtype 0 ps 1 title 'data',\
     "task0_method1-alpha-10-output.txt" using 1:2 with lines lc rgb "blue" lw 3 notitle '', '' using 1:2 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" notitle,\
     "task0_method1-alpha-10-output.txt" using 3:4 with lines lc rgb "blue" lw 3 notitle, '' using 3:4 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" notitle, \
     "task0_method1-alpha-10-output.txt" using 5:6 with lines lc rgb "blue" lw 3 title "center paths", '' using 5:6 every ::last_index with points pointtype 15 ps 0.5 lc rgb "red" title 'final centers'

set terminal pdf size 4, 2 font "Helvetica,8"
set output 'task0-loss-alpha-1.pdf'
set xlabel 'epochs'
set ylabel 'loss'
set yrange [0:]
plot "task0_method1-alpha-1-output.txt" using 7 with lines notitle

set terminal pdf size 4, 2 font "Helvetica,8"
set output 'task0-loss-alpha-10.pdf'
set xlabel 'epochs'
set ylabel 'loss'
set yrange [0:]
plot "task0_method1-alpha-10-output.txt" using 7 with lines notitle