#include "../dappservices/cron.hpp"

#define DAPPSERVICES_ACTIONS() \
  XSIGNAL_DAPPSERVICE_ACTION \
  CRON_DAPPSERVICE_ACTIONS \

#define DAPPSERVICE_ACTIONS_COMMANDS() \
  CRON_SVC_COMMANDS()

#define CONTRACT_NAME() revshare

CONTRACT_START()
  TABLE stat {
      uint64_t   counter = 0;
  };

  typedef eosio::singleton<"stat"_n, stat> stats_def;

  TABLE mapping {
      name account;
      double percentage;
      uint64_t primary_key()const {return account.value;}
  };

  typedef eosio::multi_index<"mapping"_n, mapping> mapping_def;

  TABLE usage {
      name service;
      double quota;
      uint64_t primary_key()const {return service.value;}
  };

  typedef eosio::multi_index<"usage"_n, usage> usage_def;

  bool timer_callback(name timer, std::vector<char> payload, uint32_t seconds){
      
    action(
        permission_level{get_self(),"active"_n},
        "dappservices",
        "claimrewards"_n,
        std::make_tuple("heliosselene")
      ).send();
    return true;

  }
//TODO - pull table, iterate over each row, send inline action for each contributor 
  [[eosio::on_notify("dappservices::transfer")]]
    void deposit(name hodler, name to, eosio::asset quantity, std::string memo)
    {
        if (to != get_self() || hodler == get_self())
        {
            print("These are not the droids you are looking for.");
            return;
        }

        check(now() < the_party, "You're way late");
        check(quantity.amount > 0, "When pigs fly");
        check(quantity.symbol == hodl_symbol, "These are not the droids you are looking for.");

        balance_table balance(get_self(), hodler.value);
        auto hodl_it = balance.find(hodl_symbol.raw());

        if (hodl_it != balance.end())
            balance.modify(hodl_it, get_self(), [&](auto &row) {
            row.funds += quantity;
            });
        else
            balance.emplace(get_self(), [&](auto &row) {
            row.funds = quantity;
            });
        }

    [[eosio::action]] void testschedule() {
        std::vector<char> payload;
        schedule_timer(_self, payload, 60*60*24);
  }

  // addaccount, removeaccount, updateaccount - add remove and update accounts

CONTRACT_END((testschedule))
