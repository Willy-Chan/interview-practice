// naive implementation: go over every subsequence and record the max, exp!

// 0 -> [1, 3, 2, 3...]
//      1 -> [3, 2, 3]

class Solution {
    public:
        int lis_helper(vector<int>& options, int prev_idx, int cur_idx, map<pair<int, int>, int>& mycache) {
    
            if (cur_idx >= options.size()) { 
                return 0; 
            }
            if (mycache.find({prev_idx, cur_idx}) != mycache.end()) {
                return mycache[{prev_idx, cur_idx}];
            }
    
            int chooseresult = 0;
                
            // CHOOSE AT THIS INDEX
            if (prev_idx == -1 || options[cur_idx] > options[prev_idx]) {
                chooseresult = 1 + lis_helper(options, cur_idx, cur_idx + 1, mycache);
            }
    
            // DO NOT CHOOSE THIS INDEX
            int unchooseresult = lis_helper(options, prev_idx, cur_idx + 1, mycache);
    
            mycache[{prev_idx, cur_idx}] = max(chooseresult, unchooseresult);
    
            return mycache[{prev_idx, cur_idx}];
        }
    
        int lengthOfLIS(vector<int>& nums) {
            map<pair<int, int>, int> mycache;
            return lis_helper(nums, -1, 0, mycache);
        }
    };