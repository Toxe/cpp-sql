// generated by /opt/vcpkg/installed/x64-osx/scripts/sqlpp11-ddl2cpp performance.ddl performance Performance
#ifndef PERFORMANCE_PERFORMANCE_H
#define PERFORMANCE_PERFORMANCE_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace Performance
{
  namespace Performance_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct Time
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T time;
            T& operator()() { return time; }
            const T& operator()() const { return time; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::require_insert>;
    };
    struct Request
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "request";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T request;
            T& operator()() { return request; }
            const T& operator()() const { return request; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Duration
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "duration";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T duration;
            T& operator()() { return duration; }
            const T& operator()() const { return duration; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct User
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T user;
            T& operator()() { return user; }
            const T& operator()() const { return user; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct IpAddress
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ip_address";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ipAddress;
            T& operator()() { return ipAddress; }
            const T& operator()() const { return ipAddress; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Action
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "action";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T action;
            T& operator()() { return action; }
            const T& operator()() const { return action; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct UserAgent
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "user_agent";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T userAgent;
            T& operator()() { return userAgent; }
            const T& operator()() const { return userAgent; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  } // namespace Performance_

  struct Performance: sqlpp::table_t<Performance,
               Performance_::Id,
               Performance_::Time,
               Performance_::Request,
               Performance_::Duration,
               Performance_::User,
               Performance_::IpAddress,
               Performance_::Action,
               Performance_::UserAgent>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "performance";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T performance;
        T& operator()() { return performance; }
        const T& operator()() const { return performance; }
      };
    };
  };
} // namespace Performance
#endif
