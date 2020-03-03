function rising = returnRisingEdges(vector, threshold)
    vector = (vector > threshold);
    vectorPadded = [0; vector; 0];  %pad with 0's at ends
    edges = diff(vectorPadded);
    rising = find(edges==1);
end
