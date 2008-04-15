/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2007 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 3.0 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

#ifndef __TLM_INITIATOR_SOCKET_H__
#define __TLM_INITIATOR_SOCKET_H__

//#include <systemc>
#include "tlm_h/tlm_trans/tlm_2_interfaces/tlm_fw_bw_ifs.h"

namespace tlm {

template <unsigned int BUSWIDTH,
          typename FW_IF,
          typename BW_IF> class tlm_target_socket;

template <unsigned int BUSWIDTH = 32,
          typename FW_IF = tlm_fw_transport_if<>,
          typename BW_IF = tlm_bw_transport_if<> >
class tlm_initiator_socket : public sc_core::sc_port<FW_IF>
{
public:
  typedef FW_IF                                 fw_interface_type;
  typedef BW_IF                                 bw_interface_type;
  typedef sc_core::sc_port<fw_interface_type>   port_type;
  typedef sc_core::sc_export<bw_interface_type> export_type;
  typedef tlm_target_socket<BUSWIDTH,
                            fw_interface_type,
                            bw_interface_type>  target_socket_type;

  template <unsigned int, typename, typename>
  friend class tlm_target_socket;

public:
  tlm_initiator_socket()
  : port_type(sc_core::sc_gen_unique_name("tlm_initiator_socket"))
  , mExport(sc_core::sc_gen_unique_name("tlm_initiator_socket_export"))
  {
  }

  explicit tlm_initiator_socket(const char* name)
  : port_type(name)
  , mExport(sc_core::sc_gen_unique_name((std::string(name) + "_export").c_str()))
  {
  }

  unsigned int get_bus_width() const
  {
    return BUSWIDTH;
  }

  //
  // Bind initiator socket to target socket
  // - Binds the port of the initiator socket to the export of the target
  //   socket
  // - Binds the port of the target socket to the export of the initiator
  //   socket
  //
  void bind(target_socket_type& s)
  {
    // initiator.port -> target.export
    (*static_cast<port_type*>(this))(*static_cast<typename target_socket_type::export_type*>(&s));
    // target.port -> initiator.export
    s.mPort(mExport);
  }

  void operator() (target_socket_type& s)
  {
    bind(s);
  }

  //
  // Bind initiator socket to initiator socket (hierarchical bind)
  // - Binds both the export and the port
  //
  void bind(tlm_initiator_socket& s)
  {
    // port
    (*static_cast<port_type*>(this))(*static_cast<port_type*>(&s));
    // export
    s.mExport(mExport);
  }

  void operator() (tlm_initiator_socket& s)
  {
    bind(s);
  }

  //
  // Bind interface to socket
  // - Binds the interface to the export of this socket
  //
  void bind(bw_interface_type& ifs)
  {
    mExport(ifs);
  }

  void operator() (bw_interface_type& s)
  {
    bind(s);
  }

protected:
  export_type mExport;
};

//
// Convenience socket classes
//

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm_generic_payload_types>
class tlm_conv_initiator_socket :
  public tlm_initiator_socket <BUSWIDTH,
                               tlm_fw_transport_if<TYPES>,
                               tlm_bw_transport_if<TYPES> >
{
public:
  tlm_conv_initiator_socket() :
    tlm_initiator_socket<BUSWIDTH,
                         tlm_fw_transport_if<TYPES>,
                         tlm_bw_transport_if<TYPES> >()
  {
  }

  explicit tlm_conv_initiator_socket(const char* name) :
    tlm_initiator_socket<BUSWIDTH,
                         tlm_fw_transport_if<TYPES>,
                         tlm_bw_transport_if<TYPES> >(name)
  {
  }
};

} // namespace tlm

#endif
