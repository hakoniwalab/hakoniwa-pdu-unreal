#ifndef _PDU_CTYPE_CONV_HAKO_hako_mavlink_msgs_HakoHilGps_HPP_
#define _PDU_CTYPE_CONV_HAKO_hako_mavlink_msgs_HakoHilGps_HPP_

#include "pdu_primitive_ctypes.h"
#include "ros_primitive_types.hpp"
#include "pdu_primitive_ctypes_conv.hpp"
#include "pdu_dynamic_memory.hpp"
/*
 * Dependent pdu data
 */
#include "hako_mavlink_msgs/pdu_ctype_HakoHilGps.h"
/*
 * Dependent ros data
 */
#include "hako_mavlink_msgs/msg/hako_hil_gps.hpp"

/*
 * Dependent Convertors
 */

/***************************
 *
 * PDU ==> ROS2
 *
 ***************************/

static inline int _pdu2ros_HakoHilGps(const char* heap_ptr, Hako_HakoHilGps &src, hako_mavlink_msgs::msg::HakoHilGps &dst)
{
    // primitive convert
    hako_convert_pdu2ros(src.time_usec, dst.time_usec);
    // primitive convert
    hako_convert_pdu2ros(src.lat, dst.lat);
    // primitive convert
    hako_convert_pdu2ros(src.lon, dst.lon);
    // primitive convert
    hako_convert_pdu2ros(src.alt, dst.alt);
    // primitive convert
    hako_convert_pdu2ros(src.eph, dst.eph);
    // primitive convert
    hako_convert_pdu2ros(src.epv, dst.epv);
    // primitive convert
    hako_convert_pdu2ros(src.vel, dst.vel);
    // primitive convert
    hako_convert_pdu2ros(src.vn, dst.vn);
    // primitive convert
    hako_convert_pdu2ros(src.ve, dst.ve);
    // primitive convert
    hako_convert_pdu2ros(src.vd, dst.vd);
    // primitive convert
    hako_convert_pdu2ros(src.cog, dst.cog);
    // primitive convert
    hako_convert_pdu2ros(src.satellites_visible, dst.satellites_visible);
    // primitive convert
    hako_convert_pdu2ros(src.id, dst.id);
    // primitive convert
    hako_convert_pdu2ros(src.yaw, dst.yaw);
    // primitive convert
    hako_convert_pdu2ros(src.fix_type, dst.fix_type);
    (void)heap_ptr;
    return 0;
}

static inline int hako_convert_pdu2ros_HakoHilGps(Hako_HakoHilGps &src, hako_mavlink_msgs::msg::HakoHilGps &dst)
{
    void* base_ptr = (void*)&src;
    void* heap_ptr = hako_get_heap_ptr_pdu(base_ptr);
    // Validate magic number and version
    if (heap_ptr == nullptr) {
        return -1; // Invalid PDU metadata
    }
    else {
        return _pdu2ros_HakoHilGps((char*)heap_ptr, src, dst);
    }
}

/***************************
 *
 * ROS2 ==> PDU
 *
 ***************************/

static inline bool _ros2pdu_HakoHilGps(hako_mavlink_msgs::msg::HakoHilGps &src, Hako_HakoHilGps &dst, PduDynamicMemory &dynamic_memory)
{
    try {
        // primitive convert
        hako_convert_ros2pdu(src.time_usec, dst.time_usec);
        // primitive convert
        hako_convert_ros2pdu(src.lat, dst.lat);
        // primitive convert
        hako_convert_ros2pdu(src.lon, dst.lon);
        // primitive convert
        hako_convert_ros2pdu(src.alt, dst.alt);
        // primitive convert
        hako_convert_ros2pdu(src.eph, dst.eph);
        // primitive convert
        hako_convert_ros2pdu(src.epv, dst.epv);
        // primitive convert
        hako_convert_ros2pdu(src.vel, dst.vel);
        // primitive convert
        hako_convert_ros2pdu(src.vn, dst.vn);
        // primitive convert
        hako_convert_ros2pdu(src.ve, dst.ve);
        // primitive convert
        hako_convert_ros2pdu(src.vd, dst.vd);
        // primitive convert
        hako_convert_ros2pdu(src.cog, dst.cog);
        // primitive convert
        hako_convert_ros2pdu(src.satellites_visible, dst.satellites_visible);
        // primitive convert
        hako_convert_ros2pdu(src.id, dst.id);
        // primitive convert
        hako_convert_ros2pdu(src.yaw, dst.yaw);
        // primitive convert
        hako_convert_ros2pdu(src.fix_type, dst.fix_type);
    } catch (const std::runtime_error& e) {
        std::cerr << "convertor error: " << e.what() << std::endl;
        return false;
    }
    (void)dynamic_memory;
    return true;
}

static inline int hako_convert_ros2pdu_HakoHilGps(hako_mavlink_msgs::msg::HakoHilGps &src, Hako_HakoHilGps** dst)
{
    PduDynamicMemory dynamic_memory;
    Hako_HakoHilGps out;
    if (!_ros2pdu_HakoHilGps(src, out, dynamic_memory)) {
        return -1;
    }
    int heap_size = dynamic_memory.get_total_size();
    void* base_ptr = hako_create_empty_pdu(sizeof(Hako_HakoHilGps), heap_size);
    if (base_ptr == nullptr) {
        return -1;
    }
    // Copy out on base data
    memcpy(base_ptr, (void*)&out, sizeof(Hako_HakoHilGps));

    // Copy dynamic part and set offsets
    void* heap_ptr = hako_get_heap_ptr_pdu(base_ptr);
    dynamic_memory.copy_to_pdu((char*)heap_ptr);

    *dst = (Hako_HakoHilGps*)base_ptr;
    return hako_get_pdu_meta_data(base_ptr)->total_size;
}

static inline Hako_HakoHilGps* hako_create_empty_pdu_HakoHilGps(int heap_size)
{
    // Allocate PDU memory
    char* base_ptr = (char*)hako_create_empty_pdu(sizeof(Hako_HakoHilGps), heap_size);
    if (base_ptr == nullptr) {
        return nullptr;
    }
    return (Hako_HakoHilGps*)base_ptr;
}
#endif /* _PDU_CTYPE_CONV_HAKO_hako_mavlink_msgs_HakoHilGps_HPP_ */
