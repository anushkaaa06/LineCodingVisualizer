
set terminal png size 800,300
set output 'line_coding_waveform.png'
set xlabel 'Time'
set ylabel 'Voltage Level'
set yrange [-1.5:1.5] # Adjust y-range based on your coding
set xtics 1
set ytics 1
set grid
plot 'line_coding_data.dat' with linespoints title 'Encoded Signal'
