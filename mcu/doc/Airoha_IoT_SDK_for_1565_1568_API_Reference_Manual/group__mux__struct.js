var group__mux__struct =
[
    [ "mux_buffer_t", "structmux__buffer__t.html", [
      [ "buf_size", "structmux__buffer__t.html#a50820ebe716191dad920c497510b3220", null ],
      [ "p_buf", "structmux__buffer__t.html#afeaeee72b488745390daf8d4716e63ea", null ]
    ] ],
    [ "mux_port_setting_t", "structmux__port__setting__t.html", [
      [ "dev_setting", "structmux__port__setting__t.html#a9adca09b98babf1951681db7c9516c29", null ],
      [ "flowcontrol_type", "structmux__port__setting__t.html#afc987e66f9ee344cc8f3f78879dd8ea6", null ],
      [ "portLinkRegAddr", "structmux__port__setting__t.html#a4784e3934022fa8dd22acc83d4761f8d", null ],
      [ "rx_buffer_size", "structmux__port__setting__t.html#a2540bc9a02b6dfb6fe58a8496c58afbb", null ],
      [ "slave_config", "structmux__port__setting__t.html#adc6ea0e77d94ef3dd7a62de8cd3c94ce", null ],
      [ "spi_slave", "structmux__port__setting__t.html#aa09718a77ecbe73e13ef791fa9c27c4e", null ],
      [ "tx_buffer_size", "structmux__port__setting__t.html#a851ed1e9fa8b7874990afde3a9f0a9f8", null ],
      [ "uart", "structmux__port__setting__t.html#a5d342478140814102a4b49d435ae2455", null ],
      [ "uart_config", "structmux__port__setting__t.html#ad02ec82f820dc20271b65c3b05494436", null ],
      [ "usb", "structmux__port__setting__t.html#a8bcee6021f553ebbd37ecd2b10e6353d", null ]
    ] ],
    [ "mux_port_buffer_t", "structmux__port__buffer__t.html", [
      [ "buf", "structmux__port__buffer__t.html#ac7917f5599f3e52a22f93cadeac476bf", null ],
      [ "count", "structmux__port__buffer__t.html#a86988a65e0d3ece7990c032c159786d6", null ]
    ] ],
    [ "mux_port_assign_t", "structmux__port__assign__t.html", [
      [ "count", "structmux__port__assign__t.html#a86988a65e0d3ece7990c032c159786d6", null ],
      [ "name", "structmux__port__assign__t.html#a9997d8ee2df51d18efed254a5aa016fd", null ]
    ] ],
    [ "mux_protocol_t", "structmux__protocol__t.html", [
      [ "rx_protocol_callback", "structmux__protocol__t.html#a8e2f6bae85243438e6285953b3622178", null ],
      [ "tx_protocol_callback", "structmux__protocol__t.html#a16eccf62ffcc197478c751d691d11179", null ],
      [ "user_data", "structmux__protocol__t.html#a0f53d287ac7c064d1a49d4bd93ca1cb9", null ]
    ] ],
    [ "mux_get_rx_avail_t", "structmux__get__rx__avail__t.html", [
      [ "ret_size", "structmux__get__rx__avail__t.html#af0859f9ed69a211113b98ec0d564509b", null ]
    ] ],
    [ "mux_get_tx_avail_t", "structmux__get__tx__avail__t.html", [
      [ "ret_size", "structmux__get__tx__avail__t.html#af0859f9ed69a211113b98ec0d564509b", null ]
    ] ],
    [ "mux_get_connection_param_t", "structmux__get__connection__param__t.html", [
      [ "iap2_session_id", "structmux__get__connection__param__t.html#ab5436f7f21652096f525569d42c070d6", null ],
      [ "iap2_session_num", "structmux__get__connection__param__t.html#a8413632fb564d132e925dd77dea9bb35", null ],
      [ "max_packet_size", "structmux__get__connection__param__t.html#a41dda64bb22d11831235bd7be33a0882", null ],
      [ "remote_address", "structmux__get__connection__param__t.html#a16740b274098ab3f3261c1c048930a2c", null ]
    ] ],
    [ "mux_set_config_param_t", "structmux__set__config__param__t.html", [
      [ "is_rx_need_session_id", "structmux__set__config__param__t.html#a49f77002d4348033a99cbce27deb16ca", null ],
      [ "reserved", "structmux__set__config__param__t.html#acb7bc06bed6f6408d719334fc41698c7", null ]
    ] ],
    [ "mux_get_trx_status_t", "structmux__get__trx__status__t.html", [
      [ "rx_receive_status", "structmux__get__trx__status__t.html#a67ceb38fb95cb75606c8904db067a7dd", null ],
      [ "transfer_completed_size", "structmux__get__trx__status__t.html#a5e371c661754c13312570acada120107", null ],
      [ "tx_send_status", "structmux__get__trx__status__t.html#a402cafe9797432a498eceba0001561db", null ]
    ] ],
    [ "mux_ctrl_para_t", "unionmux__ctrl__para__t.html", [
      [ "mux_get_connection_param", "unionmux__ctrl__para__t.html#a6b5051bf47a347cd0872761dcc6412d7", null ],
      [ "mux_get_rx_avail", "unionmux__ctrl__para__t.html#a74190ac7830eef49847c4804727fb789", null ],
      [ "mux_get_trx_status", "unionmux__ctrl__para__t.html#a6958542c3921a0ff9453af8f7825f700", null ],
      [ "mux_get_tx_avail", "unionmux__ctrl__para__t.html#aa77c71361430f0199cddf687e8e7b4fb", null ],
      [ "mux_set_config_param", "unionmux__ctrl__para__t.html#afd9f7399beb244f7cc6767441061232e", null ],
      [ "mux_virtual_rx_avail_len", "unionmux__ctrl__para__t.html#a15271a56bb206431b3455ab28d08eb5f", null ],
      [ "mux_virtual_tx_avail_len", "unionmux__ctrl__para__t.html#aa9b19e56af819c7661aaa3ac9c9c2544", null ]
    ] ],
    [ "TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN", "group__mux__struct.html#gacea3da4033cc1f6268dfe0ce1f94609a", null ],
    [ "TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN", "group__mux__struct.html#ga148a22f3512c7c477c7f11124ade5a29", null ],
    [ "mux_callback_t", "group__mux__struct.html#ga036450abde4ae76300aca4503b2ac41f", null ],
    [ "mux_handle_t", "group__mux__struct.html#ga7fbf32b0a859295ff9d2d7a8dbd9a62e", null ],
    [ "mux_ctrl_cmd_t", "group__mux__struct.html#ga7c4c5119e43077164075f241cdbc1aa3", [
      [ "MUX_CMD_GET_TX_AVAIL", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a99e8d3c4567344ef517c73cf9ef3fe70", null ],
      [ "MUX_CMD_GET_RX_AVAIL", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a930c3b002cde5eeabb2294d8f7c77e1c", null ],
      [ "MUX_CMD_GET_TX_BUFFER_STATUS", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a5c6b37c16b489f91b54d56dbd7dfa23d", null ],
      [ "MUX_CMD_GET_RX_BUFFER_STATUS", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a56f673e1c46f98f3899ea74cb26fc4da", null ],
      [ "MUX_CMD_CONNECT", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3abb86ac66b337e7160fce873bc3ee9b45", null ],
      [ "MUX_CMD_DISCONNECT", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a807b76865ca0dd75f4ab02b6ad7ada36", null ],
      [ "MUX_CMD_GET_CONNECTION_PARAM", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a1e216358c01d25a7dbaee712006d28ed", null ],
      [ "MUX_CMD_SET_RX_PARAM", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a9a3f47346bdb36bd1305e612fbfc86f6", null ],
      [ "MUX_CMD_CLEAN", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a18b160a24e309f7fd6ee215c752115a9", null ],
      [ "MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a1860a9076fc654d4ed5d3a83c561c874", null ],
      [ "MUX_CMD_GET_VIRTUAL_RX_AVAIL_LEN", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3acd3bdf9845ffec8d29c333b9469c0442", null ],
      [ "MUX_CMD_CLEAN_TX_VIRUTUAL", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a01e2afe40c5af1078e2dfee5da63e554", null ],
      [ "MUX_CMD_CLEAN_RX_VIRUTUAL", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a4caf58685b0675c8cdd8c6265211c147", null ],
      [ "MUX_TX_BUFFER_SEND", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3adb46c228a5201e46e2a3f4382e35851d", null ],
      [ "MUX_CMD_GET_TX_SEND_STATUS", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3afb05e99e19ef89628e7ff76e50692843", null ],
      [ "MUX_CMD_GET_RX_RECEIVED_STATUS", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3affdef6b371a4cf633489fbcaba94f16d", null ],
      [ "MUX_CMD_MAX", "group__mux__struct.html#gga7c4c5119e43077164075f241cdbc1aa3a34edc05bdd95d3f5825fed4ba791772e", null ]
    ] ]
];