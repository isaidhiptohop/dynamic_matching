%algorithm_names = [
%"global-path"
%"baswana-gupta-sen",
%"naive eps=0.5",
%"naive eps=0.1",
%"naive eps=0.01",
%"neiman-solomon"
%];

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
k_max = size(algorithms)(1) + 1;

axis_m = [0 size(intersects)(1) 0 ceil(max(max(intersects))/20)*20]

cols = size(intersects)(2)
n_is = [1:size(intersects)(1)];

no_gpa = 0;

cols_pre = size(algorithms)(1);
cols_pre = cols_pre * (cols_pre-1) * 3 / 2;

if (cols == cols_pre)
  no_gpa = 1;
  k_max -= 1;
endif

for i=1:3:cols # 3 columns for every comparision: matching1.size matching2.size intersect.size
  fig = 100+((i+2)/3);
  l = l+1;
  
  if (i >= (from-1)*3 && i <= till*3) 
    figure(fig);
    hold on;
    k
    l
    plot(n_is, intersects(n_is,i+2), "color", [0.5 0.5 0.5], "lineWidth", linewidth);
    plot(n_is, intersects(n_is,i+0), "color", color(k+no_gpa,:), "lineWidth", linewidth);
    plot(n_is, intersects(n_is,i+1), "color", color(l+no_gpa,:), "lineWidth", linewidth);
    hold off;
    title(strcat("matching intersects size", options));
  %  legend("intersect", algorithm_names(k,:), algorithm_names(l,:), "location", "southoutside");
    xlabel(strcat("sequence step x", num2str(at_once)));
    ylabel("edge set cardinality");
    axis(axis_m);

    if (exist(strcat(prefix, "img")) == 7) 
      saveas(fig, strcat(prefix, "img/intersect_", names_short(k,:), "_", names_short(l,:) ,".png"));
    endif
  endif
  
  if (l == k_max)
    k = k+1;
    l = k;
  endif
endfor
