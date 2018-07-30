algorithm_names = [
"global-path"
"baswana-gupta-sen",
"naive eps=0.5",
"naive eps=0.1",
"naive eps=0.01",
"neiman-solomon"
];

names_short = [
"gpa",
"bgs",
"naive05",
"naive01",
"naive001",
"ns"
];

k = 1;
l = 1;
k_max = 6;

axis_m = [0 size(intersects)(1) 0 ceil(max(max(intersects))/20)*20]

cols = size(intersects)(2)
n_is = [1:size(intersects)(1)];

for i=1:3:cols # 3 columns for every comparision: matching1.size matching2.size intersect.size
  fig = 100+((i+2)/3);
  l = l+1;
  
  figure(fig);
  hold on;
  plot(n_is, intersects(n_is,i+2), "color", [0.5 0.5 0.5], "lineWidth", linewidth);
  plot(n_is, intersects(n_is,i+0), "color", color(k,:), "lineWidth", linewidth);
  plot(n_is, intersects(n_is,i+1), "color", color(l,:), "lineWidth", linewidth);
  hold off;
  title(strcat("matching intersects size", options));
  legend("intersect", algorithm_names(k,:), algorithm_names(l,:), "location", "southoutside");
  xlabel(strcat("sequence step x", num2str(at_once)));
  ylabel("edge set cardinality");
  axis(axis_m);

  saveas(fig, strcat(prefix, "img/intersect_", names_short(k,:), "_", names_short(l,:) ,".png"));
  
  if (l == k_max)
    k = k+1;
    l = k;
  endif
endfor
