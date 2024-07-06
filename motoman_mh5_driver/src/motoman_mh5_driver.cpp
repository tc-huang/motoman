// References:
// [1] https://control.ros.org/master/doc/ros2_control/hardware_interface/doc/writing_new_hardware_component.html
// [2] https://github.com/ros-controls/ros2_control_demos/blob/eb8733c588aecca58d491c8389c1553880942bda/example_1/hardware/rrbot.cpp

#include "motoman_mh5_driver/motoman_mh5_driver.hpp"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"

namespace motoman_mh5_driver
{
    hardware_interface::CallbackReturn MotomanMH5Driver::on_init(const hardware_interface::HardwareInfo & info){
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_init");
        if (hardware_interface::SystemInterface::on_init(info) != CallbackReturn::SUCCESS)
        {
            return CallbackReturn::ERROR;
        }
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        // hw_start_sec_ = stod(info_.hardware_parameters["example_param_hw_start_duration_sec"]);
        // hw_stop_sec_ = stod(info_.hardware_parameters["example_param_hw_stop_duration_sec"]);
        // hw_slowdown_ = stod(info_.hardware_parameters["example_param_hw_slowdown"]);
        // END: This part here is for exemplary purposes - Please do not copy to your production code
        hw_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
        hw_commands_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());

        for (const hardware_interface::ComponentInfo & joint : info_.joints)
        {
            // RRBotSystemPositionOnly has exactly one state and command interface on each joint
            if (joint.command_interfaces.size() != 1)
            {
            RCLCPP_FATAL(
                rclcpp::get_logger("MotomanMH5Driver"),
                "Joint '%s' has %zu command interfaces found. 1 expected.", joint.name.c_str(),
                joint.command_interfaces.size());
            return hardware_interface::CallbackReturn::ERROR;
            }

            if (joint.command_interfaces[0].name != hardware_interface::HW_IF_POSITION)
            {
            RCLCPP_FATAL(
                rclcpp::get_logger("MotomanMH5Driver"),
                "Joint '%s' have %s command interfaces found. '%s' expected.", joint.name.c_str(),
                joint.command_interfaces[0].name.c_str(), hardware_interface::HW_IF_POSITION);
            return hardware_interface::CallbackReturn::ERROR;
            }

            if (joint.state_interfaces.size() != 1)
            {
            RCLCPP_FATAL(
                rclcpp::get_logger("MotomanMH5Driver"),
                "Joint '%s' has %zu state interface. 1 expected.", joint.name.c_str(),
                joint.state_interfaces.size());
            return hardware_interface::CallbackReturn::ERROR;
            }

            if (joint.state_interfaces[0].name != hardware_interface::HW_IF_POSITION)
            {
            RCLCPP_FATAL(
                rclcpp::get_logger("MotomanMH5Driver"),
                "Joint '%s' have %s state interface. '%s' expected.", joint.name.c_str(),
                joint.state_interfaces[0].name.c_str(), hardware_interface::HW_IF_POSITION);
            return hardware_interface::CallbackReturn::ERROR;
            }
        }
        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn MotomanMH5Driver::on_configure(const rclcpp_lifecycle::State & /*previous_state*/){
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_configure");
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Configuring ...please wait...");

        for (int i = 0; i < hw_start_sec_; i++)
        {
            rclcpp::sleep_for(std::chrono::seconds(1));
            RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "%.1f seconds left...",
            hw_start_sec_ - i);
        }
        // END: This part here is for exemplary purposes - Please do not copy to your production code

        // reset values always when configuring hardware
        for (uint i = 0; i < hw_states_.size(); i++)
        {
            hw_states_[i] = 0;
            hw_commands_[i] = 0;
        }

        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Successfully configured!");
        return hardware_interface::CallbackReturn::SUCCESS;
    }

    // hardware_interface::CallbackReturn MotomanMH5Driver::on_cleanup(const rclcpp_lifecycle::State & /*previous_state*/){
    //     RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_cleanup");
    //     return hardware_interface::CallbackReturn::SUCCESS;
    // }

    std::vector<hardware_interface::StateInterface>MotomanMH5Driver::export_state_interfaces(){
        std::vector<hardware_interface::StateInterface> state_interfaces;
        for (uint i = 0; i < info_.joints.size(); i++)
        {
            state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_states_[i]));
        }
        return state_interfaces;
    }

    std::vector<hardware_interface::CommandInterface>MotomanMH5Driver::export_command_interfaces(){
        std::vector<hardware_interface::CommandInterface> command_interfaces;
        for (uint i = 0; i < info_.joints.size(); i++)
        {
            command_interfaces.emplace_back(hardware_interface::CommandInterface(
            info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_commands_[i]));
        }
        return command_interfaces;
    }

    hardware_interface::CallbackReturn MotomanMH5Driver::on_activate(const rclcpp_lifecycle::State & /*previous_state*/){
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_activate");
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Activating ...please wait...");

        for (int i = 0; i < hw_start_sec_; i++)
        {
            rclcpp::sleep_for(std::chrono::seconds(1));
            RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "%.1f seconds left...",
            hw_start_sec_ - i);
        }
        // END: This part here is for exemplary purposes - Please do not copy to your production code

        // command and state should be equal when starting
        for (uint i = 0; i < hw_states_.size(); i++)
        {
            hw_commands_[i] = hw_states_[i];
        }

        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Successfully activated!");

        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn MotomanMH5Driver::on_deactivate(
    const rclcpp_lifecycle::State & /*previous_state*/)
    {
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_deactivate");
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Deactivating ...please wait...");

        for (int i = 0; i < hw_stop_sec_; i++)
        {
            rclcpp::sleep_for(std::chrono::seconds(1));
            RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "%.1f seconds left...",
            hw_stop_sec_ - i);
        }

        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Successfully deactivated!");
        // END: This part here is for exemplary purposes - Please do not copy to your production code

        return hardware_interface::CallbackReturn::SUCCESS;
    }
    // hardware_interface::CallbackReturn MotomanMH5Driver::on_shutdown(const rclcpp_lifecycle::State & /*previous_state*/){
    //     RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_shutdown");
    //     return hardware_interface::CallbackReturn::SUCCESS;
    // }

    // hardware_interface::CallbackReturn MotomanMH5Driver::on_error(const rclcpp_lifecycle::State & /*previous_state*/){
    //     RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_error");
    //     return hardware_interface::CallbackReturn::SUCCESS;
    // }

    hardware_interface::return_type MotomanMH5Driver::read(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/){
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "read");
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Reading...");

        for (uint i = 0; i < hw_states_.size(); i++)
        {
            // Simulate RRBot's movement
            hw_states_[i] = hw_states_[i] + (hw_commands_[i] - hw_states_[i]) / hw_slowdown_;
            RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Got state %.5f for joint %d!",
            hw_states_[i], i);
        }
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Joints successfully read!");
        // END: This part here is for exemplary purposes - Please do not copy to your production code
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type MotomanMH5Driver::write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/){
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "write");
        // BEGIN: This part here is for exemplary purposes - Please do not copy to your production code
        RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Writing...");

        for (uint i = 0; i < hw_commands_.size(); i++)
        {
            // Simulate sending commands to the hardware
            RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Got command %.5f for joint %d!",
            hw_commands_[i], i);
        }
        RCLCPP_INFO(
            rclcpp::get_logger("MotomanMH5Driver"), "Joints successfully written!");
        // END: This part here is for exemplary purposes - Please do not copy to your production code
        return hardware_interface::return_type::OK;
    }
}   // namespace motoman_mh5_driver

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(motoman_mh5_driver::MotomanMH5Driver, hardware_interface::SystemInterface)