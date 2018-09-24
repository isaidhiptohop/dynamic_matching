#prefix = "facebook-wosn-links_4204737162_100000_300000_meyerhenke_100000_1000_3/";
#prefix = "link-dynamic-frwiki_4204737162_10000_300000_native_1000/";
#prefix = "link-dynamic-frwiki_1234_100000_1000000_native_1000/";

# load data files
if (prefix(size(prefix)(2)) != '/')
  prefix = strcat(prefix, "/");
endif
data = load(strcat(prefix, "data"));
gpad = load(strcat(prefix, "data_gpa"));
intersects = load(strcat(prefix, "intersects"));

graphics_toolkit("gnuplot");

graph_n = load(strcat(prefix, "n"));
options = strcat(", n=",num2str(graph_n));

at_once = 10000;

linewidth = 3;

gpa       = 1;
bgs       = 2;
naive     = 3;
ns        = 4; 
rw_v1_05  = 5;
rw_v1_01  = 6;
rw_v1_001 = 7;
rw_v2_05  = 8;
rw_v2_01  = 9;
rw_v2_001 = 10;
rw_v3     = 11;
rw_v4_05  = 12;
rw_v4_01  = 13;
rw_v4_001 = 14;
rw_v5_05  = 15;
rw_v5_01  = 16;
rw_v5_001 = 17;

#bgs       = 2;
#naive05   = 3;
#naive01   = 4;
#naive001  = 5;
#ns        = 6;

color(gpa,:)        = [10 2 2]./10;
color(bgs,:)        = [2 10 4]./10;
color(naive,:)      = [10 6 2]./10;
color(ns,:)         = [10 9 0]./10;
color(rw_v1_05,:)   = [8 0 4]./10;
color(rw_v1_01,:)   = [10 2 6]./10;
color(rw_v1_001,:)  = [10 4 8]./10;
color(rw_v2_05,:)   = [0 6 6]./10;
color(rw_v2_01,:)   = [0 8 8]./10;
color(rw_v2_001,:)  = [0 10 10]./10;
color(rw_v3,:)      = [2 2 10]./10;
color(rw_v4_05,:)   = [2 10 2]./20;
color(rw_v4_01,:)   = [2 10 2]./16;
color(rw_v4_001,:)  = [2 10 2]./12;
color(rw_v5_05,:)   = [10 8 2]./15;
color(rw_v5_01,:)   = [10 8 2]./13;
color(rw_v5_001,:)  = [10 8 2]./11;

mins = [];
runtimes = [];

fig_counter = 1;
cols = 7;

for i = 0:(size(data)(2)/cols - 1)
  if (show(i+2))
  n = [1:size(data)(1)];
  
  # average node degree of matched nodes
  figure(1);
  hold on;
  plot(n, data(:,1 + i*cols)./(data(:,4 + i*cols).*2), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
  
  # average node degree in the graph
  figure(2);
  hold on;
  plot(n, (data(:,3 + i*cols).*2)./data(:,2 + i*cols), "color", [0.5 0.5 0.5], "lineWidth", linewidth);
  hold off;

  # number of nodes with degree > 1
  figure(8);
  hold on;
  plot(n, data(:,2 + i*cols), "color", [0.5 0.5 0.5], "lineWidth", linewidth);
  hold off;
  
  # plot edge cardinality of graph
  figure(3);
  hold on;
  # column 3 is the edge cardinality of the graph
  plot(n, data(:,3 + i*cols), "color", [0.5 0.5 0.5], "lineWidth", linewidth);
  hold off;
  
  # edge cardinality of matching
  figure(4);
  hold on;
  plot(n, data(:,4 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
#  plot(n, 5.*10.^(data(:,4 + i*6)./max(data(:,4 + i*6))), "color", [i/6 0.5 1-i/6], "lineWidth", 1.2);
  hold off;
  
  # average edge cardinality of matching
  figure(9);
  hold on;
  plot(n, data(:,5 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
  
  # runtime
  figure(5);
  hold on;
  plot(n, data(:,6 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  runtimes = [runtimes, data(:,6 + i*cols)];
  hold off;
  
  # cross run jaccard similarity
  figure(6);
  hold on;
  mins = [mins, min(data(:,7 + i*cols))];
  plot(n, data(:,7 + i*cols), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;

  n_gpa = [1:size(gpad)(1)];
  # speed up in comparison to gpa
  figure(7);
  hold on;
  plot(n_gpa, gpad(n_gpa,3)./(data(n_gpa,6 + i*cols)), "color", color(i+2,:), "lineWidth", linewidth);
  hold off;
  endif
endfor

figure(11)
hold on
plot(n, sqrt(data(:,3 + rw_v3*cols)), "color", color(rw_v3,:), "lineWidth", linewidth);

foo = ones(1,length(n));
plot(n, foo.*(1/0.5), "color", color(rw_v2_05,:), "lineWidth", linewidth);
plot(n, foo.*(1/0.1), "color", color(rw_v2_01,:), "lineWidth", linewidth);
plot(n, foo.*(1/0.01), "color", color(rw_v2_001,:), "lineWidth", linewidth);
hold off

title(strcat("random walk length k", options));
legend("k = sqrt(m)", "k = 1/0.5", "k = 1/0.1", "k = 1/0.01", "location", "southoutside");
ylabel("length");
xlabel(strcat("sequence step x", num2str(at_once)));

i = gpa;

figure(3);
hold on;
plot(n_gpa, gpad(:,1), "color", [0.7 0.7 0.7], "lineWidth", linewidth);
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

legends = cell;
strcat(prefix,"algorithms")
algorithms = importdata(strcat(prefix,"algorithms"));
lj = 1;

for j=1:size(show)(2)-1
  if (show(j+1)==1)
    legends{lj} = algorithms{j};
    lj = lj+1;
  endif
endfor

#figure(2);
#title(strcat("cross algorithm matching intersects size", options));
#legend("baswana-gupta-sen with naive(eps=0.5)", 
#  "naive eps=0.5 with naive(eps=0.1)", 
#  "naive eps=0.1 with naive(eps=0.01)", 
#  "naive eps=0.01 with neiman-solomon", 
#  "neiman-solomon with baswana-gupta-sen", 
#  "location", "southoutside"
#);
figure(1);
title(strcat("average node degree of matched nodes", options));
legend(legends, "location", "southoutside");
ylabel("degree");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(2);
title(strcat("average node degree in graph", options));
legend("node degree", "location", "southoutside");
ylabel("degree");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(3);
title(strcat("graph edge cardinality", options));
legend("edge cardinality", "location", "southoutside");
ylabel("edge set size");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(4);
title(strcat("matching edge cardinality", options));
legend(legends, "location", "southoutside");
ylabel("edge set size");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(9);
title(strcat("matching edge cardinality", options));
legend(legends, "location", "southoutside");
ylabel("edge set size");
xlabel(strcat("sequence step x", num2str(at_once)));

#axis([0 300 0 55]);
figure(5);
title(strcat("runtime", options));
legend(legends, "location", "southoutside");
ylabel("runtime in seconds");
xlabel(strcat("sequence step x", num2str(at_once)));
axis([0 size(data)(1) 0 quantile(quantile(runtimes, 0.95), 0.95)*1.2])#axis([0 size(data)(1) 0 max(max(runtimes))]);

figure(6);
title(strcat("cross run jaccard similarity of matchings", options));
legend(legends, "location", "southoutside");
ylabel("jaccard similarity");
xlabel(strcat("sequence step x", num2str(at_once)));

#axis([0 300 min(mins) 1.1]);
figure(7);
title(strcat("speed up against gpa", options));
legend(legends, "location", "southoutside");
ylabel("speedup");
xlabel(strcat("sequence step x", num2str(at_once)));

figure(8);
title(strcat("number of nodes with degree >= 1", options));
legend("number of nodes", "location", "southoutside");
ylabel("number of nodes");
xlabel(strcat("sequence step x", num2str(at_once)));

if (exist(strcat(prefix, "img")) == 7) 
  saveas(1, strcat(prefix, "img/average_ndegree_matched.png"));
  saveas(2, strcat(prefix, "img/average_ndegree_graph.png"));
  saveas(3, strcat(prefix, "img/graph_edge_cardinality.png"));
  saveas(4, strcat(prefix, "img/matching_edge_cardinality.png"));
  saveas(5, strcat(prefix, "img/runtime.png"));
  saveas(6, strcat(prefix, "img/cross_run_jaccard_similarity.png"));
  saveas(7, strcat(prefix, "img/speedup_against_gpa.png"));
  saveas(8, strcat(prefix, "img/active_nodes.png"));
endif