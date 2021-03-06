// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_EXTENSIONS_EXTENSION_PROCESS_XWALK_EXTENSION_PROCESS_H_
#define XWALK_EXTENSIONS_EXTENSION_PROCESS_XWALK_EXTENSION_PROCESS_H_

#include <map>
#include <string>

#include "base/values.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "ipc/ipc_channel_handle.h"
#include "ipc/ipc_listener.h"
#include "xwalk/extensions/common/xwalk_extension_permission_types.h"
#include "xwalk/extensions/common/xwalk_extension_server.h"

namespace base {
class FilePath;
}

namespace IPC {
class SyncChannel;
}

namespace xwalk {
namespace extensions {

class XWalkExtension;
class XWalkExtensionRunner;


// This class represents the Extension Process itself.
// It not only represents the extension side of the browser <->
// extension process communication channel, but also the extension side
// of the extension <-> render process channel.
// It will be responsible for handling the native side (instances) of
// External extensions through its XWalkExtensionServer.
class XWalkExtensionProcess : public IPC::Listener,
                              public XWalkExtension::PermissionsDelegate {
 public:
  XWalkExtensionProcess(
      const IPC::ChannelHandle& channel_handle = IPC::ChannelHandle());

  ~XWalkExtensionProcess() override;
  bool CheckAPIAccessControl(const std::string& extension_name,
      const std::string& api_name) override;
  bool RegisterPermissions(const std::string& extension_name,
      const std::string& perm_table) override;

 private:
  // IPC::Listener implementation.
  bool OnMessageReceived(const IPC::Message& message) override;

  // Handlers for IPC messages from XWalkExtensionProcessHost.
  void OnRegisterExtensions(const base::FilePath& extension_path,
                            const base::ListValue& browser_variables);

  void CreateBrowserProcessChannel(const IPC::ChannelHandle& channel_handle);

  void CreateRenderProcessChannel();

  base::WaitableEvent shutdown_event_;
  base::Thread io_thread_;
  std::unique_ptr<IPC::SyncChannel> browser_process_channel_;
  XWalkExtensionServer extensions_server_;
  std::unique_ptr<IPC::SyncChannel> render_process_channel_;
  IPC::ChannelHandle rp_channel_handle_;
  typedef std::map<std::string, RuntimePermission> PermissionCacheType;
  PermissionCacheType permission_cache_;

  DISALLOW_COPY_AND_ASSIGN(XWalkExtensionProcess);
};

}  // namespace extensions
}  // namespace xwalk

#endif  // XWALK_EXTENSIONS_EXTENSION_PROCESS_XWALK_EXTENSION_PROCESS_H_
