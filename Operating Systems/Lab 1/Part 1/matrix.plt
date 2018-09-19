set termoption dash
set style line 80 lt -1 lc rgb "#808080"
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"
set grid back linestyle 81
set border 3 back linestyle 80
set terminal pdfcairo size 2,3
set output '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_ijk.pdf'
set xtics nomirror
set ytics nomirror

set key top left
set style fill solid border rgb "black"
set auto x
set yrange [0:*]
set multiplot layout 3, 1
set style data histogram
set ylabel "delay(ms)"
set style fill solid border rgb "black"
set auto x
set yrange [0:*]

set title 'N = 100'
plot '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_histogram.log' using 2:xtic(1)

set title 'N = 500'
plot '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_histogram.log' using 3:xtic(1)

set title 'N = 1000'
plot '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_histogram.log' using 4:xtic(1)
unset multiplot

set terminal pdfcairo size 2,1.5
set style data linespoints
set xlabel 'N'
set key top left
set output '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_scaling.pdf'
set title 'scaling plot'
plot '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix_line.log' using 1:2 title 'i,j,k' pointsize 0.5