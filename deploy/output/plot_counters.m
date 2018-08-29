## Copyright (C) 2018 phuksz
## 
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*- 
## @deftypefn {} {@var{retval} =} plot_counters (@var{input1}, @var{input2})
##
## @seealso{}
## @end deftypefn

## Author: phuksz <phuksz@x-baby>
## Created: 2018-07-25

prefix;

counters = load(strcat(prefix, "counters"));

titles = [
"bgs in det", 
"bgs in rand", 
"repair invariant without match",
"bgs out det", 
"bgs out rand", 
"naive0.500000 match", 
"naive0.100000 match", 
"naive0.010000 match", 
"ns direct match", 
"ns resolve augpath"
];

# matches_per_step_simple0.500000 
# solve_conflict0.500000 
# matches_per_step_simple0.100000 
# solve_conflict0.100000 
# matches_per_step_simple0.010000 
# solve_conflict0.010000 
# matches_per_step_ns 
# neighbours_of_u 
# u_s 
# easy_insert_ns 
# complex_insert_aug_path 
# complex_insert_is_free 
# remove_matched_edge_ns 


counters = counters(1:(size(counters)(1)-1),:);

size_counters = size(counters)(2);
start = 1;
#size_counters = 4

for i=start:size_counters
  figure(i+100);
  plot(counters(:,i));
  
  if (exist(strcat(prefix, "img")) == 7) 
    saveas(i+200, strcat(prefix, "img/counter_", num2str(i), ".png"));
  endif
  
  if (i <= size(titles)) 
    title(titles(i,:));
  endif
endfor