clear all
clc
disp("Reading....");
m = readtable("madeUp.csv");
m = table2array(m);
disp("Done!");

%m = flip(m);

start = m(:, 3);
stop = m(:, 2);
times = m(:, 1);
threshold = 1.5;  %for example

risingStart = returnRisingEdges(start, threshold);
risingStop  = returnRisingEdges(stop, threshold);

if(length(risingStart) ~= length(risingStop))
    disp("WARNING: " + length(risingStart) + "=/=" + length(risingStop));
end

deltas = zeros(1, length(risingStop));
for index=1:length(risingStop)
    deltas(1, index) = times(risingStop(index)) - times(risingStart(index));
end
deltas = deltas';

disp("Total Amount: "+ length(deltas));

