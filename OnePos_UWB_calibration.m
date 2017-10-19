%% Initial Values needed for the calibration of the system

distance = 7.914; %Distance between the 3 nodes in meters (all distances should be the same)
%The following are the measured distance between the nodes with RX and TX
%delays set to 0. Those distances are the mean of many samples (200 for
%example)
d12 = 162.1613; %Distance between node 1 and 2 measured by node 1
d13 = 162.2531;
d21 = 162.172;
d23 = 162.2449;
d31 = 162.2155;
d32 = 162.2582;

n_iteration= 100; %number of iterations
n_candidates = 1000; %number of candidates to test in each iteration
c = 299792458; %speed of light in m/s 
Initial_delay=513e-9; %seconds
perturbation_limits = 0.2e-9; %seconds
delay_variance = 6e-9;
%% Creates the variables needed
EDM_actual = distance*ones(3).*[0 1 1; 1 0 1; 1 1 0];
EDM_measured = [0 d12 d13; d21 0 d23; d31 d32 0];

ToF_actual = EDM_actual./c;
ToF_measured = EDM_measured./c;

DWT_time = 1/(128*499.2e6);
TX_percentage = 0.44;
RX_percentage = 0.56;
%% Generate candidates and test them

aux = (Initial_delay - delay_variance);
candidates = [aux + (delay_variance*2).*rand(n_candidates,1), aux + (delay_variance*2).*rand(n_candidates,1), aux + (delay_variance*2).*rand(n_candidates,1) , zeros(n_candidates,1)];
counter = 20; %Counter to halve the perturbation limit every 20 iteration

for i=1:n_iteration
    
    if counter == 0
        counter = 20;
        perturbation_limits = perturbation_limits/2;
    end
    
    if i > 1 %add perturbation to candidates and repeat the 25 percent best
        candidates(n_candidates*0.25+1:n_candidates*0.5,:) = (candidates(1:n_candidates*0.25,:)-perturbation_limits) + (perturbation_limits*2).*rand(n_candidates*0.25,4);
        candidates(n_candidates*0.5+1:n_candidates*0.75,:) = (candidates(1:n_candidates*0.25,:)-perturbation_limits) + (perturbation_limits*2).*rand(n_candidates*0.25,4);
        candidates(n_candidates*0.75+1:n_candidates,:)     = (candidates(1:n_candidates*0.25,:)-perturbation_limits) + (perturbation_limits*2).*rand(n_candidates*0.25,4);
    end
    
    for j=1:n_candidates  %evalueate candidates
        Delay_chip1 = [ 0 candidates(j,1) candidates(j,1); candidates(j,2) 0 candidates(j,2); candidates(j,3) candidates(j,3) 0];
        Delay_chip2 = [ 0 candidates(j,2) candidates(j,3); candidates(j,1) 0 candidates(j,3); candidates(j,1) candidates(j,2) 0];
        ToF_candidate = (2*Delay_chip1 + 2*Delay_chip2 + 4*ToF_actual)/4;
        candidates(j,4) = norm(ToF_measured-ToF_candidate);
    end
    
    %Organize candidates with less error
    candidates = sortrows(candidates,4);
    
    
    counter = counter - 1;
    
end

candidates(1,:) % Result
d1_RX = candidates(1,1)*RX_percentage;
d1_TX = candidates(1,1)*TX_percentage;
d2_RX = candidates(1,2)*RX_percentage;
d2_TX = candidates(1,2)*TX_percentage;
d3_RX = candidates(1,3)*RX_percentage;
d3_TX = candidates(1,3)*TX_percentage;

d1_dwt_rx = round(d1_RX/DWT_time);
d1_dwt_tx = round(d1_TX/DWT_time);
d2_dwt_rx = round(d2_RX/DWT_time);
d2_dwt_tx = round(d2_TX/DWT_time);
d3_dwt_rx = round(d3_RX/DWT_time);
d3_dwt_tx = round(d3_TX/DWT_time);



txt = sprintf('Calibration Done\n Device 1:\n   RX delay: %gns --> %i\n   TX delay: %gns --> %i\n Device 2:\n   RX delay: %gns --> %i\n   TX delay: %gns --> %i\n Device 3:\n   RX delay: %gns --> %i\n   TX delay: %gns --> %i',d1_RX*1e9, d1_dwt_rx, d1_TX*1e9, d1_dwt_tx,d2_RX*1e9, d2_dwt_rx, d2_TX*1e9, d2_dwt_tx,d3_RX*1e9, d3_dwt_rx, d3_TX*1e9, d3_dwt_tx);
disp(txt)