/*
 TODO:
 get_mime_certificate functie moet nog corresponderen met de mime module. Hierop moet nog gewacht worden omdat de mime module nog niet geheel af is.
 get_mime_message functie moet nog corresponderen met de mime module. Hierop moet ook gewacht worden totdat de mime module af is.
 get_mime_attributes moet nog worden geschreven bij de mime_module
 AVP_PROXY_REQUEST wordt nog niet meegestuurd aan de idp module
 */

//
//  request_handler_preproxy.c
//
//
//  Created by W.A. Miltenburg on 15-05-13.
//
//

#include <freeradius-devel/radiusd.h>
#include <freeradius-devel/radius.h>
#include <freeradius-devel/modules.h>

#include "common.h"
#include "mod_mime.h"
#DEFINE AUTHENTICATION_REQUEST 1 //ACCEPT-REQUEST radius response
#DEFINE AUTHENTICATION_ACK 2 //ACCEPT-ACCEPT radius response


int handle_request(REQUEST *request, int type_request)
{
    switch (type_request) //it's allowed to handle multiple requests, the request type is based on radius responses
    {
        case AUTHENTICATION_REQUEST:
            char *certificate = get_mime_certificate();
            VALUE_PAIR *avp_certificate;
            avp_certificate = pairmake("AVP_CERTIFICATE_RADIUS",
                                       certificate, T_OP_EQ); //AVP_CERTIFICATE_RADIUS is an AVP that stores the certificate chain
            pairadd(&request->reply->vps, avp_certificate); //add AVP
            return RLM_MODULE_UPDATED;                      //we are basically saying that our AVPs are updated
            
        case AUTHENTICATION_ACK:
            
            VALUE_PAIR *vp = request->packet->vps;
            
            do {
                if (vp->attribute == AVP_PROXY_REQUEST) //detect if AVP_PROXY_REQUEST is sent by the idp module
                {
                    char *message_attributes = get_mime_attributes();
                    VALUE_PAIR *avp_attributes;
                    avp_attributes = pairmake("AVP_PROXY_ATTRIBUTES",
                                         message_attributes, T_OP_EQ); //AVP_PROXY_ATTRIBUTES is an AVP that stores the attributes
                    pairadd(&request->reply->vps, avp_attributes); //add AVP
                    return RLM_MODULE_UPDATED;                      //return statement that is needed when AVPs are updated
                }
            } while ((vp = vp -> next) != 0);
            
            
            
            
    }
    
    
}
