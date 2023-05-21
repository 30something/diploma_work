#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sys/time.h>
#include <thread>
#include <vector>

using namespace std;

const int kMaxCpuAmount = 110;
const int kMaxTasksAmount = 2010;

struct Params {
  int cpu_amount;
  int S, m1, k, R, l, x;
  double phi, psi, B;
};

class Helper {
 public:
  static int get_LB_task1(int m, vector<int> overall) {
    int n = overall.size();
    if (n <= m) return 0;

    // compute lower_bound (LB) = lower estimate to the optimal time value

    sort(overall.begin(), overall.end(), std::greater<>());
    int o1 = overall[m];
    int o2 = 0;
    for (int i = m; i < n; i++) {
      o2 += overall[i];
    }
    if (o2 % m == 0) o2 /= m;
    else o2 = o2 / m + 1;
    if (n <= 2 * m) return max(o1, o2);
    int o3 = overall[2 * m - 1] + overall[2 * m];

    return max({o1, o2, o3});
  }

  static double get_LB_task2(int m, int s, int k, vector<int> overall) {
    int n = overall.size();

    // compute lower_bound (LB) = lower estimate to the optimal time value
    // assume z = s

    sort(overall.begin(), overall.end(), std::greater<>());
    double o1 = (double)overall[0] / s;
    double o2 = 0;
    for (int i = 0; i < n; i++) {
      o2 += overall[i];
    }
    o2 /= (m - k + s * k);

    if (n <= (s - 1) * k + 1) return max(o1, o2);

    double o3 = 0;
    for (int i = (s - 1) * (k - 1); i <= (s - 1) * k; i++) {
      o3 += overall[i];
    }
    o3 /= s;
    o3 = min(o3, (double)overall[(s - 1) * k]);

    return max({o1, o2, o3});
  }

};
