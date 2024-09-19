#pragma once
#include <iostream>
#include <memory>
#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <json/json.h>
#include "../spdlog/logger.hpp"

namespace SnowK
{
    bool Serialize(const Json::Value &val, std::string &dest)
    {
        Json::StreamWriterBuilder swb;
        std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

        std::stringstream ss;
        if (sw->write(val, &ss) != 0)
        {
            LOG_ERROR("JSON serialization failed");
            return false;
        }

        dest = ss.str();

        return true;
    }

    bool DeSerialize(const std::string &src, Json::Value &val)
    {
        Json::CharReaderBuilder crb;
        std::unique_ptr<Json::CharReader> cr(crb.newCharReader());

        std::string err;
        if (cr->parse(src.c_str(), src.c_str() + src.size(), &val, &err) == false)
        {
            LOG_ERROR("JSON deserialization failed: {}", err);
            return false;
        }

        return true;
    }

    class ESIndex
    {
    public:
        ESIndex(std::shared_ptr<elasticlient::Client> &client, 
                const std::string &name, 
                const std::string &type = "_doc")
            : _name(name)
            , _type(type)
            , _client(client)
        {
            Json::Value tokenizer;
            tokenizer["tokenizer"] = "ik_max_word";

            Json::Value ik;
            ik["ik"] = tokenizer;

            Json::Value analyzer;
            analyzer["analyzer"] = ik;

            Json::Value analysis;
            analysis["analysis"] = analyzer;

            _index["settings"] = analysis;
        }

        ESIndex& Append_Properties(const std::string& key, 
                        const std::string& type = "text", 
                        const std::string& analyzer = "ik_max_word", 
                        bool enabled = true)
        {
            Json::Value field;
            field["type"] = type;
            field["analyzer"] = analyzer;

            if (enabled == false)
            {
                field["enabled"] = enabled;
            }

            _properties[key] = field;

            return *this;
        }

        bool Create(const std::string &index_id = "default_index_id")
        {
            Json::Value mappings;
            mappings["dynamic"] = true;
            mappings["properties"] = _properties;
            _index["mappings"] = mappings;

            std::string body;
            if (Serialize(_index, body) == false)
            {
                return false;
            }
            // LOG_DEBUG("{}", body);

            try
            {
                auto resp = _client->index(_name, _type, index_id, body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    LOG_ERROR("Failed to create an ES index {} with an abnormal response status code: {}", 
                              _name, resp.status_code);
                    return false;
                }
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to create ES index {}, error message: {}", _name, e.what());
                return false;
            }

            return true;
        }

    private:
        std::string _name;
        std::string _type;
        Json::Value _properties;
        Json::Value _index;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESInsert
    {
    public:
        ESInsert(std::shared_ptr<elasticlient::Client> &client, 
                 const std::string &name,
                 const std::string &type = "_doc")
            : _name(name)
            , _type(type)
            , _client(client)
        {}

        template<typename T>
        ESInsert& Append_Item(const std::string &key, const T& val)
        {
            _item[key] = val;
            return *this;
        }

        bool Insert(const std::string id = "")
        {
            std::string body;
            if (Serialize(_item, body) == false)
            {
                return false;
            }
            // LOG_DEBUG("{}", body);

            try
            {
                auto resp = _client->index(_name, _type, id, body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    LOG_ERROR("Failed to add data {} with an abnormal response status code: {}", 
                              body, resp.status_code);
                    return false;
                }
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to add data {}, error message: {}", body, e.what());
                return false;
            }

            return true;
        }

    private:
        std::string _name;
        std::string _type;
        Json::Value _item;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESRemove
    {
    public:
        ESRemove(std::shared_ptr<elasticlient::Client> &client,
                 const std::string &name,
                 const std::string &type = "_doc") 
            : _name(name)
            , _type(type)
            , _client(client) 
        {}

        bool Remove(const std::string &id)
        {
            try
            {
                auto resp = _client->remove(_name, _type, id);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    LOG_ERROR("Failed to delete data {} with an abnormal response status code: {}", 
                              id, resp.status_code);
                    return false;
                }
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to delete data {}, error message: {}", id, e.what());
                return false;
            }

            return true;
        }

    private:
        std::string _name;
        std::string _type;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESSearch
    {
    public:
        ESSearch(std::shared_ptr<elasticlient::Client> &client,
                 const std::string &name,
                 const std::string &type = "_doc") 
            : _name(name)
            , _type(type)
            , _client(client) 
        {}

        // Terms: Exact match
        ESSearch& Append_Must_Not_Terms(const std::string &key, 
                                        const std::vector<std::string> &vals)
        {
            Json::Value fields;
            for (const auto &val : vals)
            {
                fields[key].append(val);
            }

            Json::Value terms;
            terms["terms"] = fields;
            _must_not.append(terms);
            
            return *this;
        }

        // should: Word segmentation match
        ESSearch &Append_Should_Match(const std::string &key, const std::string &val)
        {
            Json::Value field;
            field[key] = val;

            Json::Value match;
            match["match"] = field;

            _should.append(match);

            return *this;
        }

        ESSearch &Append_Must_Term(const std::string &key, const std::string &val)
        {
            Json::Value field;
            field[key] = val;

            Json::Value term;
            term["term"] = field;

            _must.append(term);

            return *this;
        }

        ESSearch &Append_Must_Match(const std::string &key, const std::string &val)
        {
            Json::Value field;
            field[key] = val;

            Json::Value match;
            match["match"] = field;

            _must.append(match);

            return *this;
        }

        Json::Value Search()
        {
            Json::Value cond;
            if (_must_not.empty() == false)
            {
                cond["must_not"] = _must_not;
            }
            if (_should.empty() == false)
            {
                cond["should"] = _should;
            }
            if (_must.empty() == false)
            {
                cond["must"] = _must;
            }
            
            Json::Value query;
            query["bool"] = cond;
            Json::Value root;
            root["query"] = query;

            std::string body;
            if (Serialize(root, body) == false)
            {
                return Json::Value();
            }
            // LOG_DEBUG("{}", body);

            cpr::Response resp;
            try
            {
                resp = _client->search(_name, _type, body);
                if (resp.status_code < 200 || resp.status_code >= 300)
                {
                    LOG_ERROR("Failed to search data {} with an abnormal response status code: {}", body, resp.status_code);
                    return Json::Value();
                }
            }
            catch (std::exception &e)
            {
                LOG_ERROR("Failed to search data {}, error message: {} body", e.what());
                return Json::Value();
            }

            // LOG_DEBUG("Search response body: [{}]", resp.text);
            
            Json::Value json_ret;
            if (DeSerialize(resp.text, json_ret) == false)
            {
                return Json::Value();
            }

            return json_ret["hits"]["hits"];
        }

    private:
        std::string _name;
        std::string _type;
        Json::Value _must_not;
        Json::Value _should;
        Json::Value _must;
        std::shared_ptr<elasticlient::Client> _client;
    };
} // namespace SnowK
