#include "netclient.h"
#include "../model/data.hpp"

namespace network
{
    NetClient::NetClient(model::DataCenter *dataCenter)
        : dataCenter(dataCenter)
    {

    }

    void NetClient::Ping()
    {
        QNetworkRequest httpReq;
        httpReq.setUrl(QUrl(HTTP_URL + "/ping"));

        QNetworkReply* httpResp = httpClient.get(httpReq);

        // When the slot function is triggered, the response has returned
        connect(httpResp, &QNetworkReply::finished, this, [=]()
        {
            if (httpResp->error() != QNetworkReply::NoError)
            {
                LOG() << "The HTTP request failed: " << httpResp->errorString();
                httpResp->deleteLater();
                return;
            }

            QByteArray body = httpResp->readAll();
            LOG() << "The content of the response: " << body;

            httpResp->deleteLater();
        });
    }
} // end of namespace network
