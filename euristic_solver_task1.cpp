#include "helper.h"

using namespace std;

vector<int> greedy_list[4];
vector<int> groups_divide[2];
vector<int> dummy_list;
vector<int> lpt;
mt19937 gen32(chrono::steady_clock::now().time_since_epoch().count());

void dummy_distribution(int cpu_amount, const vector<int>& inputs) {

  // choose 1, 2, 3, ..., 1, 2, 3, ...
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  vector<int> loads[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  int cur_index = 1;
  for (int x : inputs) {
    loads[cur_index].push_back(x);
    sums[cur_index] += x;
    maxs[cur_index] = max(x, maxs[cur_index]);
    cur_index++;
    if (cur_index > cpu_amount) cur_index = 1;
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  dummy_list.push_back(gen_mx);
}

void greedy_algo1(int cpu_amount, const vector<int>& inputs) {

  // choose minimal load
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  vector<int> loads[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : inputs) {
    int min_index = 1;
    for (int i = 2; i <= cpu_amount; i++) {
      if (sums[i] - maxs[i] < sums[min_index] - maxs[min_index]) {
        min_index = i;
      }
    }
    loads[min_index].push_back(x);
    sums[min_index] += x;
    maxs[min_index] = max(x, maxs[min_index]);
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  greedy_list[1].push_back(gen_mx);
}

void greedy_algo2(int cpu_amount, const vector<int>& inputs) {

  // choose with minimal adding to system
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  vector<int> loads[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  int cload = 0;
  for (int x : inputs) {
    int diff = -1, diff_ind, nload;
    for (int i = 1; i <= cpu_amount; i++) {
      int mx = maxs[i], nsum;
      if (x > mx) nsum = sums[i];
      else nsum = sums[i] + x - mx;
      nload = nsum;
      for (int j = 1; j <= cpu_amount; j++) {
        if (i == j) continue;
        nload = max(nload, sums[j] - maxs[j]);
      }
      if (diff == -1 || nload - cload < diff) {
        diff = nload - cload;
        diff_ind = i;
      }
    }

    loads[diff_ind].push_back(x);
    sums[diff_ind] += x;
    maxs[diff_ind] = max(maxs[diff_ind], x);
    cload += diff;
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  greedy_list[2].push_back(gen_mx);
}

void greedy_algo3(int cpu_amount, const vector<int>& inputs) {

  // unused
  // combination of #2 + #1
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  vector<int> loads[cpu_amount + 1];

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  int cload = 0;
  for (int x : inputs) {
    int diff = -1, diff_ind, nload;
    for (int i = 1; i <= cpu_amount; i++) {
      int mx = maxs[i];
      if (x > mx) nload = sums[i];
      else nload = sums[i] + x - mx;
      for (int j = 1; j <= cpu_amount; j++) {
        if (i == j) continue;
        nload = max(nload, sums[j] - maxs[j]);
      }
      if (diff == -1 || nload - cload < diff || (nload - cload == diff && sums[i] < sums[diff_ind])) {
        diff = nload - cload;
        diff_ind = i;
      }
    }

    loads[diff_ind].push_back(x);
    sums[diff_ind] += x;
    maxs[diff_ind] = max(maxs[diff_ind], x);
    cload += diff;
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  greedy_list[3].push_back(gen_mx);
}

void two_groups(int cpu_amount, const vector<int>& inputs) {

  // divide on two groups with expected LB = 2

  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  int max_cpu_border = cpu_amount / 2;
  vector<int> loads[cpu_amount + 1], overall_got;
  int c1 = 0, c2 = 0;

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : inputs) {
    overall_got.push_back(x);
    int min_index = -1;
    int LB = Helper::get_LB_task1(cpu_amount, overall_got);

//    old logic
//    int cur_coef = (int)(0.8 * LB);
//    int new_coef = (int)(1 * LB);
//
//    if (x <= cur_coef) {
//      for (int i = 1; i <= max_cpu_border; i++) {
//        int nres = sums[i] + x - max(x, maxs[i]);
//        if (nres <= new_coef) {
//          min_index = i;
//          break;
//        }
//      }
//    }
//
//    if (min_index == -1) {
//      int min_res = -1;
//      for (int i = max_cpu_border + 1; i <= cpu_amount; i++) {
//        int nres = sums[i] + x - max(x, maxs[i]);
//        if (min_res == -1 || nres < min_res) {
//          min_index = i;
//          min_res = nres;
//        }
//      }
//    }
//    if (min_index == -1) min_index = 1;

    int coef1 = (int)(2 * LB);
    int coef2 = (int)(1.5 * LB);

    if (x <= coef2) {
      for (int i = 1; i <= max_cpu_border; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= coef1) {
          c1++;
          min_index = i;
          break;
        }
      }
    }

//    doesn't matter
//    for (int i = 1; i <= max_cpu_border; i++) {
//      int nres = sums[i] + x - max(x, maxs[i]);
//      if (max(x, maxs[i]) <= coef2 && nres <= coef1) {
//        c1++;
//        min_index = i;
//        break;
//      }
//    }

    if (min_index == -1) {
      c2++;
      min_index = max_cpu_border + 1;
      int saved_load = sums[min_index] + x - max(x, maxs[min_index]);
      for (int i = max_cpu_border + 2; i <= cpu_amount; i++) {
        int new_load = sums[i] + x - max(x, maxs[i]);
        if (new_load < saved_load) {
          min_index = i;
          saved_load = new_load;
        }
      }
    }

//    cout << c1 << ' ' << c2 << '\n';

    loads[min_index].push_back(x);
    sums[min_index] += x;
    maxs[min_index] = max(maxs[min_index], x);
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  groups_divide[0].push_back(gen_mx);
}

void three_groups(int cpu_amount, const vector<int>& inputs) {

  // divide on three groups with expected LB = 2

  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  int max_cpu_border_1 = cpu_amount / 3;
  int max_cpu_border_2 = 2 * cpu_amount / 3;
  vector<int> loads[cpu_amount + 1], overall_got;
  int c1 = 0, c2 = 0, c3 = 0;

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : inputs) {
    overall_got.push_back(x);
    int min_index = -1;
    int LB = Helper::get_LB_task1(cpu_amount, overall_got);

//    old logic
//    int coef1 = (int)(1 * LB);
//    int sub_coef1 = (int)(0.6 * LB);
//    int coef2 = (int)(1 * LB);
//    int sub_coef2 = (int)(1 * LB);
//
//    if (x <= sub_coef1) {
//      for (int i = 1; i <= max_cpu_border_1; i++) {
//        int nres = sums[i] + x - max(x, maxs[i]);
//        if (nres <= coef1) {
//          min_index = i;
//          break;
//        }
//      }
//    }
//
//    if (min_index == -1 && x <= sub_coef2) {
//      for (int i = max_cpu_border_1 + 1; i <= max_cpu_border_2; i++) {
//        int nres = sums[i] + x - max(x, maxs[i]);
//        if (nres <= coef2) {
//          min_index = i;
//          break;
//        }
//      }
//    }
//
//    if (min_index == -1) {
//      int min_res = -1;
//      for (int i = max_cpu_border_2 + 1; i <= cpu_amount; i++) {
//        int nres = sums[i] + x - max(x, maxs[i]);
//        if (min_res == -1 || nres < min_res) {
//          min_index = i;
//          min_res = nres;
//        }
//      }
//    }
//
//    if (min_index == -1) min_index = 1;

    int coef_proc1 = (int)(1.6 * LB);
    int coef_task1 = (int)(0.8 * LB);
    int coef_proc2 = (int)(2 * LB);
    int coef_task2 = (int)(1.8 * LB);

    // 1.8 0.8 2 1.8
    // 1.9 1 2 1.8
    // 1.6 0.8 2 1.8

    if (x <= coef_task1) {
      for (int i = 1; i <= max_cpu_border_1; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= coef_proc1) {
          c1++;
          min_index = i;
          break;
        }
      }
    }

    if (min_index == -1 && x <= coef_task2) {
      for (int i = max_cpu_border_1 + 1; i <= max_cpu_border_2; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= coef_proc2) {
          c2++;
          min_index = i;
          break;
        }
      }
    }

    if (min_index == -1) {
      c3++;
      min_index = max_cpu_border_2 + 1;
      int saved_load = sums[min_index] + x - max(x, maxs[min_index]);
      for (int i = max_cpu_border_2 + 2; i <= cpu_amount; i++) {
        int new_load = sums[i] + x - max(x, maxs[i]);
        if (new_load < saved_load) {
          min_index = i;
          saved_load = new_load;
        }
      }
    }

//    cout << c1 << ' ' << c2 << ' ' << c3 << '\n';

    loads[min_index].push_back(x);
    sums[min_index] += x;
    maxs[min_index] = max(maxs[min_index], x);
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  groups_divide[1].push_back(gen_mx);
}

void LPT(int cpu_amount, const vector<int>& inputs) {

  // LPT
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  vector<int> loads[cpu_amount + 1];
  vector<int> sorted_inputs;

  for (int x : inputs) {
    sorted_inputs.push_back(x);
  }
  sort(sorted_inputs.begin(), sorted_inputs.end());

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : sorted_inputs) {
    int min_index = 1;
    for (int i = 2; i <= cpu_amount; i++) {
      if (sums[i] - maxs[i] < sums[min_index] - maxs[min_index]) {
        min_index = i;
      }
    }
    loads[min_index].push_back(x);
    sums[min_index] += x;
    maxs[min_index] = max(x, maxs[min_index]);
  }

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  lpt.push_back(gen_mx);
}

void print_ans(const vector<int>& vc, const string& name) {
  cout << name << " = [";
  for (int i = 0; i < vc.size(); i++) {
    cout << vc[i];
    if (i < vc.size() - 1) cout << ", ";
  }
  cout << "]" << endl;
}

int main() {

  int limit_int;
  vector<int> pr = {3, 6, 10, 15, 20, 30, 40, 50, 60, 75, 90, 100};
  vector<string> strategies = {"random", "min", "max", "jumps"};
  vector<string> limits = {"n2"};
//  vector<int> pr = {75};
//  vector<string> strategies = {"random"};
//  vector<string> limits = {"n2"};

  for (const auto& limit : limits) {
    for (const auto& strategy: strategies) {

      string foldername = "tests_" + limit + "/" + strategy;
      string test_file = foldername + "/tests_inputs_" + limit + ".txt";

      if (limit == "n") limit_int = 3000;
      else if (limit == "n2") limit_int = 1000;

      for (auto cr: pr) {
        ifstream ftest(test_file);
        string proc_amount = to_string(cr), file;

        dummy_list.clear();
        for (int i = 1; i <= 3; i++) greedy_list[i].clear();
        for (int i = 0; i < 2; i++) groups_divide[i].clear();
        lpt.clear();

        for (int amount = 100; amount <= limit_int; amount += 100) {

          vector<int> overall;
          int read_amount, x;
          ftest >> read_amount;
          assert(read_amount == amount);
          for (int i = 1; i <= amount; i++) {
            ftest >> x;
            overall.push_back(x);
          }

          dummy_distribution(cr, overall);
          greedy_algo1(cr, overall);
          greedy_algo2(cr, overall);
          greedy_algo3(cr, overall);
          two_groups(cr, overall);
          three_groups(cr, overall);
          //LPT(cr, overall);
        }

        cout << "limit: " << limit << ", strategy: " << strategy <<
                ", CPUs: " << cr << endl;
        print_ans(dummy_list, "D");
        print_ans(greedy_list[1], "G1");
        print_ans(greedy_list[2], "G2");
        print_ans(greedy_list[3], "G3");
        print_ans(groups_divide[0], "GR1");
        print_ans(groups_divide[1], "GR2");
        //print_ans(lpt, "LPT");
        cout << endl;
        ftest.close();
      }
    }
  }

  return 0;
}
