#prefix = "facebook-wosn-links_4204737162_100000_300000_meyerhenke_100000_1000_3/";
#prefix = "link-dynamic-frwiki_4204737162_10000_300000_native_1000/";
#prefix = "link-dynamic-frwiki_1234_100000_1000000_native_1000/";

# load data files
data = load(strcat(prefix, "data"));
gpad = load(strcat(prefix, "data_gpa"));
intersects = load(strcat(prefix, "intersects"));

graphics_toolkit("gnuplot");

graph_n = load(strcat(prefix, "n"));
at_once = 1000;

linewidth = 2;

gpa       = 1;
bgs       = 2;
naive05   = 3;
naive01   = 4;
naive001  = 5;
ns        = 6;

color(bgs,:)      = [5 8 2]./10;
color(naive05,:)  = [1 2 4]./10;
color(naive01,:)  = [1 4 7]./10;
color(naive001,:) = [1 6 10]./10;
color(ns,:)       = [9 8 1]./10;
color(gpa,:)      = [8 2 1]./10;

mins = [];
runtimes = [];

fig_counter = 1;
cols = 6;

for i = 0:(size(data)(2)/cols - 1)
  n = [1:size(data)(1)];

  figure(2);
  hold on;
  plot(n, data(:,2 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
  
  # plot edge cardinality of graph
  figure(3);
  hold on;
  # column 3 is the edge cardinality of the graph
  plot(n, data(:,3 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
  
  # edge cardinality of matching
  figure(4);
  hold on;
  plot(n, data(:,4 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
#  plot(n, 5.*10.^(data(:,4 + i*6)./max(data(:,4 + i*6))), "color", [i/6 0.5 1-i/6], "lineWidth", 1.2);
  hold off;
  
  # runtime
  figure(5);
  hold on;
  plot(n, data(:,5 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  runtimes = [runtimes, data(:,5 + i*cols)];
  hold off;
  
  # cross run jaccard similarity
  figure(6);
  hold on;
  mins = [mins, min(data(:,6 + i*cols))];
  plot(n, data(:,6 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;

  n_gpa = [1:size(gpad)(1)];
  # speed up in comparison to gpa
  figure(7);
  hold on;
  plot(n_gpa, gpad(n_gpa,3)./data(n_gpa,5 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
endfor

i = gpa;

figure(3);
hold on;
plot(n_gpa, gpad(:,1), "color", color(i,:), "lineWidth", linewidth);
hold off;

figure(4);
hold on;
plot(n_gpa, gpad(:,2), "color", color(i,:), "lineWidth", linewidth);
#plot(n, 5.*10.^(gpad(:,2)./max(gpad(:,2))), "color", [i/6 0.5 1-i/6], "lineWidth", 1.2);
hold off;

figure(5);
hold on;
plot(n_gpa, gpad(:,3), "color", color(i,:), "lineWidth", linewidth);
hold off;

options = strcat(", n=",num2str(graph_n));

#figure(2);
#title(strcat("cross algorithm matching intersects size", options));
#legend("baswana-gupta-sen with naive(eps=0.5)", 
#  "naive eps=0.5 with naive(eps=0.1)", 
#  "naive eps=0.1 with naive(eps=0.01)", 
#  "naive eps=0.01 with neiman-solomon", 
#  "neiman-solomon with baswana-gupta-sen", 
#  "location", "southoutside"
#);
figure(3);
title(strcat("graph edge cardinality", options));
legend("baswana-gupta-sen", "naive eps=0.5", "naive eps=0.1", "naive eps=0.01", "neiman-solomon", "global path", "location", "southoutside");
ylabel("edge set size");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(4);
title(strcat("matching edge cardinality", options));
legend("baswana-gupta-sen", "naive eps=0.5", "naive eps=0.1", "naive eps=0.01", "neiman-solomon", "global path", "location", "southoutside");
ylabel("edge set size");
xlabel(strcat("sequence step x", num2str(at_once)));

#axis([0 300 0 55]);
figure(5);
title(strcat("runtime", options));
legend("baswana-gupta-sen", "naive eps=0.5", "naive eps=0.1", "naive eps=0.01", "neiman-solomon", "global path", "location", "southoutside");
ylabel("runtime in seconds");
xlabel(strcat("sequence step x", num2str(at_once)));
axis([0 size(data)(1) 0 quantile(quantile(runtimes, 0.95), 0.95)*2])#axis([0 size(data)(1) 0 max(max(runtimes))]);

figure(6);
title(strcat("cross run jaccard similarity of matchings", options));
legend("baswana-gupta-sen", "naive eps=0.5", "naive eps=0.1", "naive eps=0.01", "neiman-solomon", "location", "southoutside");
ylabel("jaccard similarity");
xlabel(strcat("sequence step x", num2str(at_once)));

#axis([0 300 min(mins) 1.1]);
figure(7);
title(strcat("speed up against gpa", options));
legend("baswana-gupta-sen", "naive eps=0.5", "naive eps=0.1", "naive eps=0.01", "neiman-solomon", "location", "southoutside");
ylabel("speedup");
xlabel(strcat("sequence step x", num2str(at_once)));

#saveas(2, strcat(prefix, "img/cross_algorithm_matching_intersects.png"));
saveas(3, strcat(prefix, "img/graph_edge_cardinality.png"));
saveas(4, strcat(prefix, "img/matching_edge_cardinality.png"));
saveas(5, strcat(prefix, "img/runtime.png"));
saveas(6, strcat(prefix, "img/cross_run_jaccard_similarity.png"));
saveas(7, strcat(prefix, "img/speedup_against_gpa.png"));
