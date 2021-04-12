#include <iostream>
#include <vector>
#include <algorithm>

template<typename T>
class Deque {
private:
    std::vector<T *> buffer_;
    size_t buffer_size_ = 0;
    std::pair<size_t, size_t> start_;
    std::pair<size_t, size_t> end_;
public:
    Deque() {
      T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
      buffer_.push_back(newarr);
    }

    Deque(int x) {
      buffer_size_ = x;
      start_.first = 0;
      start_.second = 0;
      end_.first = x / 8;
      end_.second = (7 + x) % 8;
      for (size_t i = 0; i <= end_.first; ++i) {
        T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
        int pos_2 = 8;
        if (i == end_.first - 1) {
          pos_2 = end_.second + 1;
        }
        for (int j = 0; j < pos_2; ++j) {
          new(newarr + j) T();
        }
        buffer_.push_back(newarr);
      }
    }

    Deque(const Deque &deq) {
      T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
      buffer_.push_back(newarr);
      for (size_t i = 0; i < deq.size(); ++i) {
        (*this).push_back(deq[i]);
      }
    }

    void swap(Deque &another) {
      std::swap(another.buffer_size_, buffer_size_);
      std::swap(another.start_, start_);
      std::swap(another.end_, end_);
      std::swap(another.buffer_, buffer_);
    }

    Deque &operator=(Deque deq) {
      swap(deq);
      return *this;
    }

    void update() {
      std::vector<T *> newbuffer;
      for (size_t i = 0; i < buffer_.size(); ++i) {
        T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
        newbuffer.push_back(newarr);
      }
      for (size_t i = 0; i < buffer_.size(); ++i) {
        T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
        if (i >= start_.first && i <= end_.first) {
          size_t pos_1 = 0;
          size_t pos_2 = 7;
          if (i == start_.first) {
            pos_1 = start_.second;
          }
          for (size_t j = pos_1; j <= pos_2; ++j) {
            new(newarr + j) T(buffer_[i][j]);
            (buffer_[i] + j)->~T();
          }
        }
        newbuffer.push_back(newarr);
      }
      for (size_t i = 0; i < buffer_.size(); ++i) {
        T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
        newbuffer.push_back(newarr);
      }
      buffer_ = newbuffer;
      start_.first += buffer_.size() / 3;
      end_.first += buffer_.size() / 3;
    }

    void push_back(const T &value) {
      if (buffer_size_ == 0) {
        new(buffer_[0])T(value);
        start_.first = 0;
        start_.second = 0;
        end_.first = 0;
        end_.second = 0;
      } else {
        if (end_.first == (buffer_.size() - 1) && end_.second >= 6) {
          update();
        }
        if (end_.second < 7) {
          new(buffer_[end_.first] + end_.second + 1) T(value);
          ++end_.second;
        } else {
          ++end_.first;
          end_.second = 0;
          new(buffer_[end_.first])T(value);
        }
      }
      ++buffer_size_;
    }

    void push_front(const T &value) {
      if (buffer_size_ == 0) {
        new(buffer_[0])T(value);
        start_.first = 0;
        start_.second = 0;
        end_.first = 0;
        end_.second = 0;
      } else {
        if (start_.first <= 1 && start_.second <= 1) {
          update();
        }
        if (start_.second > 0) {
          new(buffer_[start_.first] + start_.second - 1) T(value);
          --start_.second;
        } else {
          --start_.first;
          start_.second = 7;
          new(buffer_[start_.first] + 7)T(value);
        }
      }
      ++buffer_size_;
    }

    void pop_back() {
      (buffer_[end_.first] + end_.second)->~T();
      --buffer_size_;
      if (end_.second > 0) {
        --end_.second;
      } else {
        --end_.first;
        end_.second = 7;
      }
    }

    void pop_front() {
      (buffer_[start_.first] + start_.second)->~T();
      --buffer_size_;
      if (start_.second < 7) {
        ++start_.second;
      } else {
        ++start_.first;
        start_.second = 0;
      }
    }

    Deque(int n, const T &value) {
      T *newarr = reinterpret_cast<T *>(new int8_t[8 * sizeof(T)]);
      buffer_.push_back(newarr);
      for (int i = 0; i < n; ++i) {
        (*this).push_back(value);
      }
    }

    T &operator[](size_t i) {
      size_t pos_1 = (i + start_.second) / 8;
      size_t pos_2 = (i + start_.second) % 8;
      return buffer_[start_.first + pos_1][pos_2];
    }

    T &at(size_t i) {
      size_t pos_1 = (i + start_.second) / 8;
      size_t pos_2 = (i + start_.second) % 8;
      if (i >= buffer_size_) {
        throw std::out_of_range("...");
      }
      return buffer_[start_.first + pos_1][pos_2];
    }

    const T &operator[](size_t i) const {
      size_t pos_1 = (i + start_.second) / 8;
      size_t pos_2 = (i + start_.second) % 8;
      return buffer_[start_.first + pos_1][pos_2];
    }

    const T &at(size_t i) const {
      size_t pos_1 = (i + start_.second) / 8;
      size_t pos_2 = (i + start_.second) % 8;
      if (i >= buffer_size_) {
        throw std::out_of_range("...");
      }
      return buffer_[start_.first + pos_1][pos_2].value_;
    }

    size_t size() const {
      return buffer_size_;
    }

    template<bool is_const>
    class common_iterator {
    private:
        friend class Deque;

        using ptr_type = std::conditional<is_const, const T *, T *>;
        typename std::conditional<is_const, const T *, T *>::type ptr_;
        const std::vector<T *> *deq_;
        std::pair<size_t, size_t> place_;
    public:
        common_iterator(typename std::conditional<is_const, const T *, T *>::type ptr, const std::vector<T *> *deq,
                        std::pair<size_t, size_t> place) : ptr_(ptr), deq_(deq), place_(place) {}

        typename std::conditional<is_const, const T &, T &>::type operator*() {
          return *ptr_;
        }

        typename std::conditional<is_const, const T *, T *>::type operator->() {
          return ptr_;
        }

        common_iterator &operator++() {
          if (place_.second < 7) {
            ++ptr_;
            ++place_.second;
            return *this;
          }
          ++place_.first;
          place_.second = 0;
          ptr_ = (*deq_)[place_.first];
          return *this;
        }

        common_iterator &operator--() {
          if (place_.second > 0) {
            --ptr_;
            --place_.second;
            return *this;
          }
          --place_.first;
          place_.second = 7;
          ptr_ = (*deq_)[place_.first] + 7;
          return *this;
        }

        common_iterator operator++(int) {
          common_iterator copy = *this;
          ++(*this);
          return copy;
        }

        common_iterator operator--(int) {
          common_iterator copy = *this;
          --(*this);
          return copy;
        }

        common_iterator operator+(int x) {
          if (place_.second + x < 8) {
            place_.second += x;
            ptr_ += x;
            return *this;
          }
          place_.first += (place_.second + x) / 8;
          place_.second += x;
          place_.second %= 8;
          ptr_ = (*deq_)[place_.first] + place_.second;
          return *this;
        }

        common_iterator operator-(int x) {
          if (place_.second >= size_t(x)) {
            place_.second -= x;
            ptr_ -= x;
            return *this;
          }
          int y = place_.first * 8 + place_.second - x;
          place_.first = y / 8;
          place_.second = y % 8;
          ptr_ = (*deq_)[place_.first] + place_.second;
          return *this;
        }

        bool operator==(const common_iterator &another_it) const {
          return place_ == another_it.place_;
        }

        bool operator!=(const common_iterator &another_it) const {
          return !(*this == another_it);
        }

        bool operator<(const common_iterator &another_it) const {
          return (place_.first < another_it.place_.first || (place_.first == another_it.place_.first
                                                             && place_.second < another_it.place_.second));
        }

        bool operator>(const common_iterator &another_it) const {
          return (another_it < *this);
        }

        bool operator<=(const common_iterator &another_it) const {
          return ((*this < another_it) || (*this == another_it));
        }

        bool operator>=(const common_iterator &another_it) const {
          return (another_it <= *this);
        }

        size_t operator-(const common_iterator &another) const {
          size_t pos_1 = place_.first * 8 + place_.second;
          size_t pos_2 = another.place_.first * 8 + another.place_.second;
          return pos_1 - pos_2;
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
      return iterator(buffer_[start_.first] + start_.second, &buffer_, start_);
    }

    const_iterator begin() const {
      return const_iterator(buffer_[start_.first] + start_.second, &buffer_, start_);
    }

    const_iterator cbegin() const {
      return const_iterator(buffer_[start_.first] + start_.second, &buffer_, start_);
    }

    iterator end() {
      std::pair<size_t, size_t> pos = end_;
      ++pos.second;
      if (pos.second == 8) {
        pos.second = 0;
        pos.first += 1;
      }
      return iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }

    const_iterator end() const {
      std::pair<size_t, size_t> pos = end_;
      ++pos.second;
      if (pos.second == 8) {
        pos.second = 0;
        pos.first += 1;
      }
      return const_iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }


    const_iterator cend() const {
      std::pair<size_t, size_t> pos = end_;
      ++pos.second;
      if (pos.second == 8) {
        pos.second = 0;
        pos.first += 1;
      }
      return const_iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }

    reverse_iterator rbegin() {
      return reverse_iterator(buffer_[end_.first] + end_.second, &buffer_, end_);
    }

    const_reverse_iterator rbegin() const {
      return const_reverse_iterator(buffer_[end_.first] + end_.second, &buffer_, end_);
    }

    const_reverse_iterator crbegin() const {
      return const_reverse_iterator(buffer_[end_.first] + end_.second, &buffer_, end_);
    }

    reverse_iterator rend() {
      std::pair<size_t, size_t> pos = start_;
      --pos.second;
      if (pos.second == -1) {
        pos.second = 7;
        pos.first -= 1;
      }
      return reverse_iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }

    const_reverse_iterator rend() const {
      std::pair<size_t, size_t> pos = start_;
      --pos.second;
      if (pos.second == -1) {
        pos.second = 7;
        pos.first -= 1;
      }
      return const_reverse_iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }

    const_reverse_iterator crend() const {
      std::pair<size_t, size_t> pos = start_;
      --pos.second;
      if (pos.second == -1) {
        pos.second = 7;
        pos.first -= 1;
      }
      return const_reverse_iterator(buffer_[pos.first] + pos.second, &buffer_, pos);
    }


    void insert(const iterator &it, T x) {
      iterator it_2 = iterator(it.ptr_, it.deq_, it.place_);
      if (end_.first == (buffer_.size() - 1) && end_.second >= 6) {
        update();
      }
      int count = buffer_size_ - ((it.place_.first - start_.first) * 8 + (it.place_.second - start_.second));
      for (int i = 0; i < count; ++i) {
        T y = buffer_[it_2.place_.first][it_2.place_.second];
        buffer_[it_2.place_.first][it_2.place_.second] = x;
        x = y;
        ++it_2;
      }
      new(buffer_[it_2.place_.first] + it_2.place_.second) T(x);
      ++buffer_size_;
      ++end_.second;
      if (end_.second == 8) {
        end_.second = 0;
        ++end_.first;
      }
    }

    void erase(const iterator &it) {
      iterator it_2 = iterator(it.ptr_, it.deq_, it.place_);
      int count = buffer_size_ - ((it.place_.first - start_.first) * 8 + (it.place_.second - start_.second));
      for (int i = 0; i < count - 1; ++i) {
        int pos_1 = it_2.place_.first;
        int pos_2 = it_2.place_.second;
        ++it_2;
        buffer_[pos_1][pos_2] = buffer_[it_2.place_.first][it_2.place_.second];
      }
      (buffer_[it_2.place_.first] + it_2.place_.second)->~T();
      if (end_.second == 0) {
        end_.second = 7;
        --end_.first;
      } else {
        --end_.second;
      }
      --buffer_size_;
    }
};