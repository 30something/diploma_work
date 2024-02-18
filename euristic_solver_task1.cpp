#include "helper.h"

using namespace std;

vector<int> greedy_list[4];
vector<int> groups_divide[2];
vector<int> dummy_list;
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
      if (sums[i] < sums[min_index]) {
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

  // divide on two groups with LB = 1
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  int max_cpu_border = cpu_amount / 2;
  vector<int> loads[cpu_amount + 1], overall_got;

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : inputs) {
    int cur_coef = (int)(0.8 * Helper::get_LB_task1(cpu_amount, overall_got));
    int new_coef = (int)(1 * Helper::get_LB_task1(cpu_amount, overall_got));
    int min_index = -1;

    if (x <= cur_coef) {
      for (int i = 1; i <= max_cpu_border; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= new_coef) {
          min_index = i;
          break;
        }
      }
    }

    if (min_index == -1) {
      int min_res = -1;
      for (int i = max_cpu_border + 1; i <= cpu_amount; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (min_res == -1 || nres < min_res) {
          min_index = i;
          min_res = nres;
        }
      }
    }
    if (min_index == -1) min_index = 1;

    overall_got.push_back(x);
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

  // divide on three groups
  int sums[cpu_amount + 1];
  int maxs[cpu_amount + 1];
  int max_cpu_border_right = cpu_amount / 2 + 1;
  int max_cpu_border_left = max_cpu_border_right / 2;
  vector<int> loads[cpu_amount + 1], overall_got;

  for (int i = 1; i <= cpu_amount; i++) {
    sums[i] = maxs[i] = 0;
  }

  string str;
  for (int x : inputs) {

    int LB = Helper::get_LB_task1(cpu_amount, overall_got);
    int coef1 = (int)(1 * LB);
    int sub_coef1 = (int)(0.6 * LB);
    int coef2 = (int)(1 * LB);
    int sub_coef2 = (int)(1 * LB);
    int min_index = -1;

    if (x <= sub_coef1) {
      for (int i = 1; i <= max_cpu_border_left; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= coef1) {
          min_index = i;
          break;
        }
      }
    }

    if (min_index == -1 && x <= sub_coef2) {
      for (int i = max_cpu_border_left + 1; i <= max_cpu_border_right; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (nres <= coef2) {
          min_index = i;
          break;
        }
      }
    }

    if (min_index == -1) {
      int min_res = -1;
      for (int i = max_cpu_border_right + 1; i <= cpu_amount; i++) {
        int nres = sums[i] + x - max(x, maxs[i]);
        if (min_res == -1 || nres < min_res) {
          min_index = i;
          min_res = nres;
        }
      }
    }

    if (min_index == -1) min_index = 1;

    overall_got.push_back(x);
    loads[min_index].push_back(x);
    sums[min_index] += x;
    maxs[min_index] = max(maxs[min_index], x);
  }

//  int LB = get_LB(cpu_amount, overall_got);
//  int k1 = 0, k2 = 0, k3 = 0;
//  cout << LB << '\n';
//  for (auto f : overall_got) {
//    if (f <= 0.2 * LB) k1++;
//    else if (f <= 0.4 * LB) k2++;
//    else k3++;
//  }
//
//  cout << k1 << ' ' << k2 << ' ' << k3 << '\n';

  int gen_mx = -1;
  for (int i = 1; i <= cpu_amount; i++) {
    int sum = sums[i] - maxs[i];
    gen_mx = max(gen_mx, sum);
  }

  groups_divide[1].push_back(gen_mx);
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
  vector<int> pr = {3, 6, 10, 15};
  vector<string> strategies = {"random", "min", "max", "jumps"};
  vector<string> limits = {"n", "n2"};

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
        }

        cout << "limit: " << limit << ", strategy: " << strategy <<
                ", CPUs: " << cr << endl;
        print_ans(dummy_list, "D");
        print_ans(greedy_list[1], "G1");
        print_ans(greedy_list[2], "G2");
        print_ans(greedy_list[3], "G3");
        print_ans(groups_divide[0], "GR1");
        print_ans(groups_divide[1], "GR2");
        cout << endl;
        ftest.close();
      }
    }
  }

  return 0;
}
