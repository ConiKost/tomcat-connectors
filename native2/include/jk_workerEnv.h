/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *          Copyright (c) 1999-2001 The Apache Software Foundation.          *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "Jk",  and  "Apache  Software     *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

/***************************************************************************
 * Description: Workers controller header file                             *
 * Author:      Gal Shachor <shachor@il.ibm.com>                           * 
 * Version:     $Revision$                                           *
 ***************************************************************************/

#ifndef JK_WORKERENV_H
#define JK_WORKERENV_H

#include "jk_logger.h"
#include "jk_endpoint.h"
#include "jk_worker.h"
#include "jk_map.h"
#include "jk_uriMap.h"
#include "jk_uriEnv.h"
#include "jk_handler.h"
#include "jk_service.h"
#include "jk_vm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct jk_worker;
struct jk_endpoint;
struct jk_env;
struct jk_uriMap;
struct jk_uriEnv;
struct jk_map;
struct jk_logger;
struct jk_handler;
struct jk_ws_service;

/*
 * Jk configuration and global methods. 
 * 
 * Env Information to be provided to worker at init time
 * With AJP14 support we need to have access to many informations
 * about web-server, uri to worker map....
 */
struct jk_workerEnv {
    /* The URI to WORKER map. Set via JkMount in httpd.conf,
       uriworker.properties or autoconfiguration.

       It is empty if 'native' mapping is used ( SetHandler )
    */
    /*     struct jk_map *uri_to_context; */

    /* Use this pool for all global settings
     */
    struct jk_pool *pool;
    
    /* Active workers hashtable. 
     */
    struct jk_map *worker_map;

    struct jk_env *globalEnv;

    /** Worker that will be used by default, if no other
        worker is specified. Usefull for SetHandler or
        to avoid the lookup
    */
    struct jk_worker *defaultWorker;
    
    /* Web-Server we're running on (Apache/IIS/NES).
     */
    char *server_name;
    
    /* XXX Virtual server handled - "*" is all virtual
     */
    char *virtual;

    /** Initialization properties, set via native options or workers.properties.
     */
    /* XXX renamed from init_data to force all code to use setProperty
       This is private property !
    */
    struct jk_map *initData;

    /** Root env, used to register object types, etc
     */
    struct jk_env *rootEnv;

    /*
     * Log options. Extracted from init_data.
     */
    char *log_file;
    int  log_level;

    char     *secret_key;
    /*     jk_map_t *automount; */

    struct jk_uriMap *uriMap;

    struct jk_uriEnv *rootWebapp;

    /** If 'global' server mappings will be visible in virtual hosts
        as well. XXX Not sure this is needed
    */
    int      mountcopy;
    
    int was_initialized;

    /*
     * SSL Support
     */
    int  ssl_enable;
    char *https_indicator;
    char *certs_indicator;
    char *cipher_indicator;
    char *session_indicator;  /* Servlet API 2.3 requirement */
    char *key_size_indicator; /* Servlet API 2.3 requirement */

    /*
     * Jk Options
     */
    int options;

    /** Old REUSE_WORKER define. Instead of using an object pool, use
        thread data to recycle the connection. */
    int perThreadWorker;
    
    /*
     * Environment variables support
     */
    int envvars_in_use;
    struct jk_map * envvars;

    /* Handlers. This is a dispatch table for messages, for
     * each message id we have an entry containing the jk_handler_t.
     * lastMessageId is the size of the table.
     */
    struct jk_handler **handlerTable;
    int lastMessageId;

    /* The vm - we support a single instance per process
     * ( i.e can't have both jdk1.1 and jdk1.2 at the same time,
     *  or 2 instances of the same vm. )
     */
    struct jk_vm *vm;
    
    /** Private data, associated with the 'real' server
     *  server_rec * in apache
     */
    void *_private;
    
    /* -------------------- Methods -------------------- */
    /** Set a jk property. This is similar with the mechanism
     *  used by java side ( with individual setters for
     *  various properties ), except we use a single method
     *  and a big switch
     *
     *  As in java beans, setting a property may have side effects
     *  like changing the log level or reading a secondary
     *  properties file.
     *
     *  Changing a property at runtime will also be supported for
     *  some properties.
     *  XXX Document supported properties as part of
     *  workers.properties doc.
     *  XXX Implement run-time change in the status/ctl workers.
     */
    int (*setProperty)( struct jk_env *env,
                         struct jk_workerEnv *_this,
                         const char *name, char *value);

    char *(*getProperty)( struct jk_env *env,
                          struct jk_workerEnv *_this,
                          const char *name);

    /** Return a list of supported properties.
        Not all properties can be set ( some may be runtime
        status ).
        @experimental This is not a final API, I would use
        an external config ( DTD/schema or even MIB-like ?  )
    */
    char **(*getPropertyNames)( struct jk_env *env,
                                struct jk_workerEnv *_this );

    
    /** Get worker by name
     */
    struct jk_worker *(*getWorkerForName)(struct jk_env *env,
                                          struct jk_workerEnv *_this,
                                          const char *name);

    
    struct jk_worker *(*createWorker)(struct jk_env *env,
                                      struct jk_workerEnv *_this,
                                      const char *name, 
                                      struct jk_map *init_data);

    /** Call the handler associated with the message type.
     */
    int (*dispatch)(struct jk_env *env, struct jk_workerEnv *_this,
                    struct jk_endpoint *e, struct jk_ws_service *r );

    /** Utility method for stream-based workers. It'll read
     *  messages, dispatch, send the response if any until
     *  done. This assumes one native server thread talking
     *  with a different client thread ( on the java side ).
     *  It does not work for jni or doors or other transports
     *  where a single thread is used for the whole processing.
     */
    int (*processCallbacks)(struct jk_env *env,
                            struct jk_workerEnv *_this,
                            struct jk_endpoint *e,
                            struct jk_ws_service *r );

    /**
     *  Init the workers, prepare the worker environment. Will read
     *  all properties and set the jk acordignly.
     * 
     *  Replaces wc_open
     */
    int (*init)(struct jk_env *env, struct jk_workerEnv *_this);

    /** Close all workers, clean up
     *
     */
    void (*close)(struct jk_env *env, struct jk_workerEnv *_this);
};


typedef struct jk_workerEnv jk_workerEnv_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JK_WORKERENV_H */
