#include "../dappservices/cron.hpp"

#define DAPPSERVICES_ACTIONS() \
  XSIGNAL_DAPPSERVICE_ACTION \
  CRON_DAPPSERVICE_ACTIONS \

#define DAPPSERVICE_ACTIONS_COMMANDS() \
  CRON_SVC_COMMANDS()

#define CONTRACT_NAME() revshare

CONTRACT_START()

  TABLE mapping {
      name developer_account;
      double percentage;
  };

  typedef eosio::singleton<"mapping"_n, mapping> mapping_def_t;

    struct usage_t{
        asset quantity;
        name provider;
        name payer;
        name service;
        name package;
        bool success = false;
  };

  bool timer_callback(name timer, std::vector<char> payload, uint32_t seconds){
      
    action(
        permission_level{get_self(),"active"_n},
        "dappservices"_n,
        "claimrewards"_n,
        std::make_tuple(get_self()) //Auto generate name
      ).send();
    return true;

  }

//TODO - pull table, iterate over each row, send inline action for each contributor 
  [[eosio::on_notify("dappservices::transfer")]]
    void send_payment(name from, name to, eosio::asset quantity, std::string memo)
    {

          if (to != get_self() || from != name{"dappservices"} || get_code() == name{"dappservices"})
        {
            return;
        }

        usage_t usage_report;

        mapping_def_t mapping_def(_self, usage_report.service.value);

        if(!mapping_def.exists()) {
            return;
        }

        mapping new_mapping = mapping_def.get();
        name account;
        new_mapping.developer_account = account;
        double percentage;
        new_mapping.percentage = percentage;

        //check what percantage should go to developer, then send payment to developer account
        asset payment;
        payment.amount = percentage * quantity.amount;
        payment.symbol = quantity.symbol;
        // how to convert and mult quantity
        
        action(
            permission_level{get_self(),"active"_n},
            "dappservices"_n,
            "transfer"_n,
            std::make_tuple(get_self(), name{account}, payment, "your rev share") //Auto generate name
        ).send();
    }

    [[eosio::action]] void testschedule() {
        std::vector<char> payload;
        schedule_timer(_self, payload, 60*60*24);
    }

    [[eosio::action]] void set(name Account,double Percentage, name service) {
        // Add account
        mapping_def_t mapping_def(_self, service.value);
        mapping new_mapping;
        new_mapping.developer_account = Account;
        new_mapping.percentage = Percentage;
        
        mapping_def.set(new_mapping, _self); // mixed?
  }

    [[eosio::action]] void remove(name Account, name service) {
        //Remove an accountmapping_def_t mapping_def(_self, usage_report.service.value);
        
        mapping_def_t mapping_def(_self, service.value);
        mapping_def.remove(); // mixed?
  }

CONTRACT_END((testschedule)(set)(remove))
