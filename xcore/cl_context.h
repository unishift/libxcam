/*
 * cl_context.h - CL context
 *
 *  Copyright (c) 2015 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Wind Yuan <feng.yuan@intel.com>
 */

#ifndef XCAM_CL_CONTEXT_H
#define XCAM_CL_CONTEXT_H

#include "xcam_utils.h"
#include "smartptr.h"
#include <map>
#include <list>
#include <CL/cl.h>
#include <CL/cl_intel.h>

namespace XCam {

class CLKernel;
class CLDevice;
class CLCommandQueue;

/* correct usage
 *  SmartPtr<CLContext> context = CLDevice::instance()->get_context();
 */

class CLContext {
    //typedef std::map<std::string, SmartPtr<CLKernel>> CLKernelMap;
    typedef std::list<SmartPtr<CLCommandQueue>> CLCmdQueueList;

    friend class CLDevice;
    friend class CLKernel;
    friend class CLMemory;
    friend class CLVaImage;
public:
    enum KernelBuildType {
        KERNEL_BUILD_BINARY = 0,
        KERNEL_BUILD_SOURCE,
    };

    ~CLContext ();
    cl_context get_context_id () {
        return _context_id;
    }
    void terminate ();

private:
    static void context_pfn_notify (
        const char* erro_info, const void *private_info,
        size_t cb, void *user_data);
    static void program_pfn_notify (
        cl_program program, void *user_data);

    explicit CLContext (SmartPtr<CLDevice> &device);
    SmartPtr<CLCommandQueue> create_cmd_queue (SmartPtr<CLContext> &self);
    cl_kernel generate_kernel_id (
        CLKernel *kernel,
        const uint8_t *source,
        size_t length,
        KernelBuildType type);
    void destroy_kernel_id (cl_kernel &kernel_id);
    XCamReturn execute_kernel (
        CLKernel *kernel,
        CLCommandQueue *queue = NULL,
        const cl_event *events_wait = NULL,
        uint32_t num_of_events_wait = 0,
        cl_event *event_out = NULL);
    //bool insert_kernel (SmartPtr<CLKernel> &kernel);

    bool init_context ();
    void destroy_context ();
    bool is_valid () const {
        return (_context_id != NULL);
    }

    bool init_cmd_queue (SmartPtr<CLContext> &self);
    SmartPtr<CLCommandQueue> get_default_cmd_queue ();

    //Memory, Image
    cl_mem create_va_image (const cl_libva_image &image_info);
    void destroy_mem (cl_mem mem_id);

    XCAM_DEAD_COPY (CLContext);

private:
    cl_context                  _context_id;
    SmartPtr<CLDevice>          _device;
    //CLKernelMap                 _kernel_map;
    CLCmdQueueList              _cmd_queue_list;
};

class CLCommandQueue {
    friend class CLContext;

public:
    virtual ~CLCommandQueue ();
    cl_command_queue get_cmd_queue_id () {
        return _cmd_queue_id;
    }
    XCamReturn execute_kernel (SmartPtr<CLKernel> &kernel);

private:
    explicit CLCommandQueue (SmartPtr<CLContext> &context, cl_command_queue id);
    void destroy ();
    XCAM_DEAD_COPY (CLCommandQueue);

private:
    SmartPtr<CLContext>     _context;
    cl_command_queue        _cmd_queue_id;
};

};

#endif //XCAM_CL_CONTEXT_H