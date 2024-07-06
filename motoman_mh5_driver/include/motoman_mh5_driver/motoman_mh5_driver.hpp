#ifndef MOTOMAN_MH5_DRIVER_
#define MOTOMAN_MH5_DRIVER_

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"

#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"

#include "visibility_control.hpp"

namespace motoman_mh5_driver
{
    class MotomanMH5Driver: public hardware_interface::SystemInterface
    {
    public:
        RCLCPP_SHARED_PTR_DEFINITIONS(MotomanMH5Driver)

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo & info) override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State & previous_state) override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State & previous_state) override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & previous_state) override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::return_type read(const rclcpp::Time & time, const rclcpp::Duration & period) override;

        MOTOMAN_MH5_DRIVER_PUBLIC
        hardware_interface::return_type write(const rclcpp::Time & time, const rclcpp::Duration & period) override;

    private:
        // Parameters for simulation
        double hw_start_sec_;
        double hw_stop_sec_;
        double hw_slowdown_;

        // Store the command for the simulated robot
        std::vector<double> hw_commands_;
        std::vector<double> hw_states_;
    };
} // namespace motoman_mh5_driver

#endif // MOTOMAN_MH5_DRIVER_