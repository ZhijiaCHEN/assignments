set termoption dash
set style line 80 lt -1 lc rgb "#808080"
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"
set grid back linestyle 81
set border 3 back linestyle 80
set terminal pdfcairo size 2,1
set output '/Users/zhijia/git/assignments/Operating Systems/Lab 1/Part 1/matrix.pdf'
set style data histogram
set style histogram errorbars gap 1 
set xtics nomirror# offset 0,0.5
set ytics nomirror# offset 0.5, 0
set ylabel "delay(ms)"# font ",16" offset 4,0 
set xlabel "scenario"# font ",16" offset 0,1.5
set key top left
set style fill solid border rgb "black"
set auto x
set yrange [0:*]
#set datafile separator ","
#set bars 2
plot '/Users/zhijia/git/Wiser/output/MIRO_policy_errorbar.txt' using 2:3:xtic(1)
