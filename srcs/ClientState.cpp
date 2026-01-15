#include "../includes/ClientState.hpp"

ClientState::ClientState(): fd_client(-1), fd_cgi(-1), cgi_pid(-1), request_buffer(""), response_buffer(""), state(IDLE)
{

}