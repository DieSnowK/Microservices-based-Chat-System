#include "mysql.hpp"
#include "message.hpp"
#include "message-odb.hxx"

namespace SnowK
{
    class MessageTable
    {
    public:
        using ptr = std::shared_ptr<MessageTable>;

        MessageTable(const std::shared_ptr<odb::core::database> &db) 
            : _db(db) 
        {}
        ~MessageTable() {}

        bool Insert(Message &msg)
        {
            try
            {
                odb::transaction trans(_db->begin());
                _db->persist(msg);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to add a new message {}:{}", msg.Message_Id(), e.what());
                return false;
            }

            return true;
        }

        bool Remove(const std::string &ssid)
        {
            typedef odb::query<Message> query;
            try
            {
                odb::transaction trans(_db->begin());
                _db->erase_query<Message>(query::session_id == ssid);
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to delete all messages in the conversation {}:{}", ssid, e.what());
                return false;
            }

            return true;
        }

        std::vector<Message> Recent(const std::string &ssid, int count)
        {
            typedef odb::query<Message> query;
            typedef odb::result<Message> result;

            std::vector<Message> ret;
            try
            {
                odb::transaction trans(_db->begin());

                std::stringstream cond;
                cond << "session_id='" << ssid << "' order by create_time desc limit " << count;

                result r(_db->query<Message>(cond.str()));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(*i);
                }

                std::reverse(ret.begin(), ret.end());
                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get recent messages:{}-{}-{}", ssid, count, e.what());
            }

            return ret;
        }

        // Obtain information for a specified time period for a specified session
        std::vector<Message> Range(const std::string &ssid,
                                   boost::posix_time::ptime &stime,
                                   boost::posix_time::ptime &etime)
        {
            typedef odb::query<Message> query;
            typedef odb::result<Message> result;

            std::vector<Message> ret;
            try
            {
                odb::transaction trans(_db->begin());

                result r(_db->query<Message>(query::session_id == ssid &&
                                             query::create_time >= stime &&
                                             query::create_time <= etime));
                for (auto i(r.begin()); i != r.end(); ++i)
                {
                    ret.push_back(*i);
                }

                trans.commit();
            }
            catch (const std::exception &e)
            {
                LOG_ERROR("Failed to get interval messages:{}-{}:{}-{}", ssid,
                          boost::posix_time::to_simple_string(stime),
                          boost::posix_time::to_simple_string(etime), e.what());
            }
            
            return ret;
        }

    private:
        std::shared_ptr<odb::core::database> _db;
    };
}