#include <czmq.h>

int main (void)
{
    //  Create context and start authentication engine
    zctx_t *ctx = zctx_new ();
    zauth_t *auth = zauth_new (ctx);
    zauth_set_verbose (auth, true);
    zauth_allow (auth, "192.168.232.129");
    
    //  Tell authenticator to use the certificate store in .curve
    zauth_configure_curve (auth, "*", ".curve");
    
    //  We'll generate a new client certificate and save the public part
    //  in the certificate store (in practice this would be done by hand
    //  or some out-of-band process).
    zcert_t *client_cert = zcert_new ();
    zsys_dir_create (".curve");
    zcert_set_meta (client_cert, "name", "Client test certificate");
    zcert_save_public (client_cert, ".curve/testcert.pub");
    
    //  Prepare the server certificate as we did in Stonehouse
    zcert_t *server_cert = zcert_new ();
    char *server_key = zcert_public_txt (server_cert);
    
    //  Create and bind server socket
/*    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zcert_apply (server_cert, server);
    zsocket_set_curve_server (server, 1);
    zsocket_bind (server, "tcp://*:9000");
*/
    //  Create and connect client socket
    void *client = zsocket_new (ctx, ZMQ_PULL);
    zcert_apply (client_cert, client);
    zsocket_set_curve_serverkey (client, server_key);
    zsocket_connect (client, "tcp://192.168.232.129:9000");
    
    //  Send a single message from server to client
    zstr_send (client, "Hello");
    char *message = zstr_recv (client);
    if(message != NULL) {
      puts(message);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Ironhouse test OK");
    }

    zcert_destroy (&client_cert);
    //zcert_destroy (&server_cert);
    zauth_destroy (&auth);
    zctx_destroy (&ctx);
    return 0;
}
