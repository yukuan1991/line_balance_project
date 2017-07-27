#include "data_balance.h"
#include <assert.h>
#include <QDebug>
#include <tuple>
#include <utility>

std::set<int> data_balance::get_post_items (int item) const
{
    std::set<int> ret;
    for (auto & iter : data_)
    {
        const auto & tmp = std::get<2> (iter.second);
        if (tmp.find (item) != tmp.end())
        {
            ret.insert (iter.first);
        }
    }
    return ret;
}

bool data_balance::find_loop_helper (int item, std::list<int>& black_list, std::set<int>& success) const
{
    if (success.find (item) != success.end())
    {
        return true;
    }

    auto post_items = this->get_post_items (item);
    if (post_items.empty())
    {
        success.insert (item);
        return true;
    }

    for (auto iter : post_items)
    {
        for (auto iter_list : black_list)
        {
            if (iter_list == iter)
            {
                black_list.emplace_back (iter);
                while (*black_list.begin() != black_list.back())
                {
                    black_list.pop_front ();
                }
                return false;
            }
        }
    }

    for (auto iter : post_items)
    {
        black_list.emplace_back (iter);

        if (find_loop_helper (iter, black_list, success))
        {
            black_list.pop_back ();
        }
        else
        {
            return false;
        }

    }

    success.insert (item);
    return true;
}


void data_balance::clear()
{
    this->data_.clear ();
}

void data_balance::print() const
{
    auto lamda_print = [] (decltype (data_)::const_iterator p)
    {
        std::string str_set;
        std::for_each (std::begin (std::get<2> (p->second)), std::end (std::get<2> (p->second)), [&] (int k)
        {
            str_set += QString::number (k).toStdString () + " ";
        });

        qDebug () << p->first << " " << std::get<0> (p->second).data () << " " << std::get<1> (p->second) << " " << str_set.data ();
    };

    for (auto iter = std::begin (data_); iter != std::end (data_); ++iter)
    {
        lamda_print (iter);
    }
}

std::set<int> data_balance::get_front_items() const
{
    std::set<int> ret;
    for (auto & iter : data_)
    {
        if (std::get<2> (iter.second).empty ())
        {
            ret.insert (iter.first);
        }
    }

    return ret;
}


bool data_balance::add_operation(int key, std::string str_name, float f_std_time, std::set<int> set_immediate_prepecessor)
{
    if (data_.find (key) != data_.end ())
    {
        return false;
    }

    data_[key] = decltype (data_)::mapped_type {str_name, f_std_time, set_immediate_prepecessor};

    return true;
}


bool data_balance::find_loop (std::list<int>& list_loop) const
{
    for (auto item : data_)
    {
        std::set<int> success;
        for (auto iter : data_)
        {
            list_loop.clear ();
            list_loop.emplace_back (item.first);
            if (success.find (iter.first) != success.end())
            {
                continue;
            }
            auto ret = find_loop_helper (item.first, list_loop, success);
            if (ret == false)
            {
                return !ret;
            }
        }
        list_loop.clear();
    }

    return false;
}

structure_data data_balance::auto_balance_helper(float takt_time, int flag) const
{
    structure_data auto_assign {};

    decltype (takt_time) time_left = 0;

    auto follower_data = find_follower ();

    typename std::decay<decltype (*this)>::type structure = *this;

    while (!structure.get().empty())
    {
        auto front_items = structure.get_front_items ();

        assert (!front_items.empty());

        decltype (front_items) item_same_station {};
        for (auto iter : front_items)
        {
            auto item_info = structure.get ().find (iter);
            assert (item_info != structure.get().end ());

            auto std_time = std::get<1> (structure.get().find (iter)->second);

            if (std_time <= time_left)
            {
                item_same_station.insert (iter);
            }
        }

        bool next_station = item_same_station.empty ();

        if (next_station)
        {
            auto_assign.emplace_back (typename decltype (auto_assign)::value_type {});
            time_left = takt_time;
        }

        auto& item_candidate = next_station ? front_items : item_same_station;

        std::function<std::set<int> (decltype(item_candidate)&)> lamda_time_most = [&] (decltype (item_candidate)& items)
        {
            float time_top = 0;
            for (auto iter : items)
            {
                auto iter_data = structure.get().find (iter);
                assert (iter_data != structure.get().end());
                if (std::get<1> (iter_data->second) < time_top)
                {
                    continue;
                }

                time_top = std::get<1> (iter_data->second);
            }

            typename std::decay<decltype (items)>::type ret;

            for (auto iter : items)
            {
                auto iter_data = structure.get().find (iter);
                assert (iter_data != structure.get().end());
                if (std::get<1> (iter_data->second) < time_top)
                {
                    continue;
                }
                ret.insert (iter);
            }
            assert (!ret.empty ());

            return ret;
        };

        decltype (lamda_time_most) lamda_follower_most = [&] (decltype (item_candidate)& items)
        {
            unsigned follower_top = 0;
            for (auto iter : items)
            {
                auto iter_data = follower_data.find (iter);
                assert (iter_data != follower_data.end());
                if (iter_data->second.size () < follower_top)
                {
                    continue;
                }

                follower_top = iter_data->second.size ();
            }

            typename std::decay<decltype (items)>::type ret;

            for (auto iter : items)
            {
                auto iter_data = follower_data.find (iter);
                assert (iter_data != follower_data.end ());
                if (iter_data->second.size() < follower_top)
                {
                    continue;
                }
                ret.insert (iter);
            }

            assert (!ret.empty ());
            return ret;
        };
        std::vector<decltype (lamda_time_most)> vec_filter;
        if (flag == 1)
        {
            vec_filter.emplace_back (lamda_time_most);
            vec_filter.emplace_back (lamda_follower_most);
        }
        else
        {
            vec_filter.emplace_back (lamda_follower_most);
            vec_filter.emplace_back (lamda_time_most);
        }

        std::set<int> tmp;
        std::set<int> result = item_candidate;
        for (auto & iter : vec_filter)
        {
            tmp = iter (result);
            result = std::move (tmp);
        }

        auto iter_item = structure.get().find (*result.begin());
        auto_assign.back ().emplace_back (item_info{iter_item->first, std::get<0> (iter_item->second), std::get<1> (iter_item->second)});
        time_left -= std::get<1> (iter_item->second);


        structure.erase_operation (*result.begin ());
    }

    return auto_assign;
}

std::map<int, std::set<int>> data_balance::find_follower()const
{
    std::vector<int> front_items;
    for (auto & iter : this->data_)
    {
        if (std::get<2> (iter.second).empty ())
        {
            front_items.emplace_back (iter.first);
        }
    }
    std::map<int, std::set<int>> ret;

    for (auto & iter : front_items)
    {
        find_follower_helper (iter, ret);
    }
    return ret;
}

std::tuple<structure_data, float, float, float, unsigned> data_balance::auto_balance(float takt_time)
{
    auto result_1 = auto_balance_helper (takt_time, 1);
    auto result_2 = auto_balance_helper (takt_time, 0);


    decltype (takt_time) sigma_ct, max_ct1, max_ct2;

    auto get_balance_rate = [&](decltype (result_1)& input, decltype(takt_time)& max_ct)
    {
        sigma_ct = 0;
        max_ct = 0;
        for (auto & iter : input)
        {
            decltype (takt_time) tmp_maxct {};
            for (auto & iter_ : iter)
            {
                tmp_maxct += iter_.time;
                sigma_ct += iter_.time;
            }
            if (tmp_maxct > max_ct)
            {
                max_ct = tmp_maxct;
            }
        }
        qDebug () << "sigma_ct" << sigma_ct;

        return sigma_ct / max_ct / static_cast<float> (input.size ());
    };

    auto rate1 = get_balance_rate (result_1, max_ct1);
    auto rate2 = get_balance_rate (result_2, max_ct2);

    auto efficiency1 = sigma_ct / takt_time / static_cast<float> (result_1.size());
    auto efficiency2 = sigma_ct / takt_time / static_cast<float> (result_2.size());

    auto ret = rate1 > rate2 ? std::make_tuple (std::move (result_1), rate1, max_ct1, efficiency1, result_1.size ()) :
                               std::make_tuple (std::move (result_2), rate2, max_ct2, efficiency2, result_2.size ());

    auto new_size = static_cast<unsigned>(sigma_ct / takt_time * 2);

    if (new_size > std::get<0> (ret).size ())
    {
        std::get<0>(ret).resize (new_size);
    }

    return ret;
}

void data_balance::find_follower_helper(int key, std::map<int, std::set<int>> & hash_found) const
{
    if (hash_found.find (key) != hash_found.end ())
    {
        return;
    }

    auto post = this->get_post_items (key);
    auto followers = post;

    for (auto iter : post)
    {
        this->find_follower_helper (iter, hash_found);
        followers << hash_found[iter];
    }

    hash_found[key] = std::move (followers);
}


bool data_balance::erase_operation(int key)
{
    auto iter = this->data_.find (key);
    if (iter == this->data_.end ())
    {
        return false;
    }

    this->data_.erase (iter);

    for (auto & iter : this->data_)
    {
        std::set<int>& precedence = std::get<2> (iter.second);
        auto iter_element = precedence.find (key);
        if (iter_element != precedence.end ())
        {
            precedence.erase (iter_element);
        }
    }

    return true;
}
