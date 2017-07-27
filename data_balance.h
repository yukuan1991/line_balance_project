#ifndef DATA_BALANCE_H
#define DATA_BALANCE_H

#include <memory>
#include <algorithm>
#include <set>
#include <map>
#include <functional>
#include <vector>
#include <list>
#include <station_area.h>

class data_balance
{
public:
    data_balance () = default;
    data_balance (const data_balance&) = default;
    data_balance (data_balance&& that):data_ (std::move (that.data_)){}
    compl data_balance () = default;

    data_balance& operator= (const data_balance& rhs) {this->data_ = rhs.data_; return *this;}
    data_balance& operator = (data_balance&& rhs) {this->data_ = std::move (rhs.data_); return *this;}


    std::map<int, std::set<int>> find_follower ()const;

    std::tuple<structure_data, float, float, float, unsigned> auto_balance (float takt_time);

    bool add_operation (int key, std::string str_name, float f_std_time, std::set<int> set_immediate_prepecessor);

    bool erase_operation (int key);

    bool find_loop (std::list<int>& list_loop) const;


    void clear ();

    void print () const;

    const std::map<unsigned, std::tuple<std::string, float, std::set<int>>>& get () const { return data_;}

    std::set<int> get_front_items () const;

private:
    std::map<unsigned, std::tuple<std::string, float, std::set<int>>> data_ = {};
private: //functions
    structure_data auto_balance_helper (float takt_time, int flag) const;
    std::set<int> get_post_items (int item) const;
    int find_loop_at_back (int item, std::set<int> black_list) const;
    bool find_loop_helper (int item, std::list<int>& black_list, std::set<int>& success) const;
    void find_follower_helper (int, std::map<int, std::set<int>>&) const;

protected:
};

template<typename T>
inline std::set<T> operator+ (const std::set<T>& lhs, const std::set<T>& rhs)
{
    std::set<T> ret;
    std::set_union (lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::inserter(ret, ret.begin()));
    return ret;
}

template<typename T>
inline std::set<T>& operator<< (std::set<T>& lhs, const std::set<T>& rhs)
{
    for (auto & iter : rhs)
    {
        lhs.insert (iter);
    }
    return lhs;
}


#endif // DATA_BALANCE_H
