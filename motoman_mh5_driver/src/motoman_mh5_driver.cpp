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
        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_init");
        if (hardware_interface::SystemInterface::on_init(info) != CallbackReturn::SUCCESS)
        {
            return CallbackReturn::ERROR;
        }

        hw_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
        hw_commands_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
        action = false;

        // for (const hardware_interface::ComponentInfo & joint : info_.joints)
        // {
            
        // }

        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn MotomanMH5Driver::on_configure(const rclcpp_lifecycle::State & /*previous_state*/){
        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_configure");

        // reset values always when configuring hardware
        for (uint i = 0; i < hw_states_.size(); i++)
        {
            hw_states_[i] = 0;
            hw_commands_[i] = 0;
        }

        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Successfully configured!");
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
        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_activate");

        // command and state should be equal when starting
        for (uint i = 0; i < hw_states_.size(); i++)
        {
            hw_commands_[i] = hw_states_[i];
        }

        command.init();
        command.power_on(IP, PORT);

        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "Successfully activated!");

        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn MotomanMH5Driver::on_deactivate(
    const rclcpp_lifecycle::State & /*previous_state*/)
    {
        // RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "on_deactivate");
        command.power_off();
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
        if(true)
        {
            RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "read");
            command.read_joint();
            for(int i = 0; i < 6; i++){
                hw_states_[i] = command.radians[i];
            }
        }
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type MotomanMH5Driver::write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/){
        for(int i = 0; i < 6; i++){
            if (abs(hw_commands_[i] - hw_states_[i]) >= 0.02)// && action == false)
            {
                RCLCPP_INFO(rclcpp::get_logger("MotomanMH5Driver"), "write");
                command.write_joint(hw_commands_[0], hw_commands_[1], hw_commands_[2], hw_commands_[3], hw_commands_[4], hw_commands_[5], SPEED);
                // action = true;
                break;
            }
            // else action = false;
        }
        return hardware_interface::return_type::OK;
    }
}   // namespace motoman_mh5_driver

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(motoman_mh5_driver::MotomanMH5Driver, hardware_interface::SystemInterface)