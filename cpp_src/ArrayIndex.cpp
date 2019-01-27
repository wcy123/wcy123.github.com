#include <array>
#include <iostream>
#include <memory>
using namespace std;
struct DimensionComponent {
  int range_;
  int extra_size_;
  int head_;
  int tail_;
  DimensionComponent *up;
  DimensionComponent *down;
};

struct DimensionStride {
  // 有三种形式
  // 1. x * stride_[0] 正常模式
  // 1. x * stride_[0] - offset_ 畸形开始
  // 2. x * stride_[x_idx]; 畸形结束
  // x_idx == 0 if idx_ == -1
  // x_idx == 0 if sub[idx_] == 0
  // x_idx == 1 if sub[idx_] > 0 && sub[idx_] < limit_ + 1
  // x_idx == 2 if sub[idx_] == limit_ + 1;
  int stride_[3];
  int offset_;
  int idx_;
  int limit_;
  template<size_t N>
  int calculate_stride(const array<int, N> &) const;
};
template <size_t N>
struct HighDimension {
  int sub2idx(const array<int, N> &sub) const;
  array<int, N> idx2sub(int idx) const;
  const array<DimensionStride, N> strides_;
};
template <size_t N>
struct DimensionBuilder {
  DimensionBuilder<N + 1> add(int range);
  HighDimension<N> build();
  array<unique_ptr<DimensionComponent>, N - 1> components_;
};

//
template <size_t N>
DimensionBuilder<N + 1> DimensionBuilder<N>::add(int range) {  //
  auto components = array<unique_ptr<DimensionComponent>, N>{};
  for (auto i = 0u; i < N - 1; ++i) {
    components[i] = std::move(components_[i]);
  }
  components[N - 1] =
      make_unique<DimensionComponent>(range, 0, 0, 0, nullptr, nullptr);
  return DimensionBuilder{components};
}
inline int calculate_size(const DimensionComponent * from,
                          const DimensionComponent * to)
{
  int ret = 1;
  if(from == to){
    ret = 1;
  } else if (from->down == nullptr) {
    ret = (from->range_ + from->extra_size_) * calculate_size(from + 1, to);
  } else {
    auto down = from->down;
    auto unit = calculate_size(from + 1, from->down);
    ret = unit;
    ret *= from->range_ * (down->range_ + down->extra_size_) +  //
           (down->head_ + down->extra_size_) +                  //
           (down->tail_ + down->extra_size_);
    ret *= calculate_size(from->down + 1, to);
  }
  return ret;
}
template <size_t N>
HighDimension<N> DimensionBuilder<N>::build() {
  auto strides = array<DimensionStride, N>{};
  return HighDimension<N>{strides};
}
template <size_t N>
inline int DimensionStride::calculate_stride(const array<int, N> &x) const {  //
  auto stride_idx = 0;
  if (idx_ == -1) {
    stride_idx = 0;
  } else {
    if (x[idx_] == 0) {
      stride_idx = 0;
    } else if (x[idx_] < limit_ + 1) {
      stride_idx = 1;
    } else {
      stride_idx = 2;
    }
  }
  return stride_[stride_idx];
}

template <size_t N>
int HighDimension<N>::sub2idx(const array<int, N> &x) const {
  auto ret = 0;
  for (auto i = 0u; i < N; ++i) {
    const auto &s = strides_[i];
    ret += x[i] == 0 ? 0 : x[i] * s.template calculate_stride<N>(x) - s.offset_;
  }
  return ret;
}

template <size_t N>
array<int, N> HighDimension<N>::idx2sub(int idx) const {
  auto ret = array<int, N>{};
  for (auto i = 0u; i < N; ++i) {
    const auto &s = strides_[i];
    auto stride = s.template calculate_stride<N>(ret);
    ret[i] = (idx + s.offset_) / stride;
    idx -= ret[i] == 0 ? 0 : ret[i] * stride - s.offset_;
  }
  return ret;
}

int main(int argc, char *argv[]) { //
  auto hd = HighDimension<4>{
      //
      DimensionStride{{105}, 0, -1, 0},        //
      DimensionStride{{35, 0, 0}, 21, -1, 0},  //
      DimensionStride{{2, 5, 3}, 0, 1, 2},     // 7
      DimensionStride{{1, 1, 1}, 0, -1, 0},    // 5^2_3
  };
  int dim[] = {2, 5, 5, 3};
  for (int x0 = 0; x0 < 2; ++x0) {
    for (int x1 = 0; x1 < 4; ++x1) {
      for (int x2 = 0; x2 < 7; ++x2) {
        for (int x3 = 0; x3 < dim[x1]; ++x3) {
          auto sub = array<int, 4>{x0, x1, x2, x3};
          auto idx = hd.sub2idx(sub);
          auto x = hd.idx2sub(idx);
          cout << __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__ << "] "
               << "sub = (" << x0 << "," << x1 << "," << x2 << "," << x3 << ")"
               << "idx " << idx << " "
               << "sub = (" << x[0] << "," << x[1] << "," << x[2] << "," << x[3]
               << ")" << endl;
        }
      }
    }
  }
  return 0;
}

// Local Variables:
// mode:c++
// c-basic-offset: 2
// coding: utf-8-unix
// End:
