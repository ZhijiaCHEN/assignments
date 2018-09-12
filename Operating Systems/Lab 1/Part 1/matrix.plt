set termoption dash
set style line 80 lt -1 lc rgb "#808080"
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"
set grid back linestyle 81
set border 3 back linestyle 80
set terminal pdfcairo size 2,1
set output '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix.pdf'
set xtics nomirror# offset 0,0.5
set ytics nomirror# offset 0.5, 0

set key top left
set style fill solid border rgb "black"
set auto x
set yrange [0:*]
set multiplot layout 2, 1 title "i-j-k plot"
set style data histogram
set style histogram cluster gap 1
set ylabel "delay(ms)"# font ",16" offset 4,0 
set style fill solid border rgb "black"
set auto x
set yrange [0:*]
plot '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_plot.log' using 2:xtic(1) title col, \
        '' using 3:xtic(1) title col, \
        '' using 4:xtic(1) title col
