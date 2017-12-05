% make 
    will compile both programs.

% chat_server 8000

% client_cli 127.0.0.1 8000

Commands implemented:

   REQCHATROOM
   REQUUID
   NICK
   CHANGECHATROOM
   REQCHATROOMS
   NAMECHATROOM
   SENDTEXT
   REQTEXT
   REQUSERS

in one window:

./client_cli 127.0.0.1 8000 <test_case_1

and in another:

./chat_server 8000
