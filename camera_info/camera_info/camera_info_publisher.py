import rclpy
from rclpy.node import Node
from sensor_msgs.msg import CameraInfo
import yaml

class CameraInfoPublisher(Node):
    def __init__(self):
        super().__init__('camera_info_publisher')

        # 读取 YAML 文件
        yaml_filename = '/motoman/install/camera_info/share/camera_info/ost.yaml'
        with open(yaml_filename, 'r') as file:
            camera_info_data = yaml.safe_load(file)

        # 创建 CameraInfo 消息
        self.camera_info_msg = CameraInfo()
        self.camera_info_msg.width = camera_info_data['image_width']
        self.camera_info_msg.height = camera_info_data['image_height']
        self.camera_info_msg.k = camera_info_data['camera_matrix']['data']
        self.camera_info_msg.d = camera_info_data['distortion_coefficients']['data']
        self.camera_info_msg.r = camera_info_data['rectification_matrix']['data']
        self.camera_info_msg.p = camera_info_data['projection_matrix']['data']
        self.camera_info_msg.distortion_model = camera_info_data['distortion_model']

        # 创建发布者
        self.publisher_ = self.create_publisher(CameraInfo, 'camera_info', 10)

        # 创建定时器，定时发布消息
        self.timer = self.create_timer(0.5, self.publish_camera_info)

    def publish_camera_info(self):
        self.publisher_.publish(self.camera_info_msg)

def main(args=None):
    rclpy.init(args=args)
    node = CameraInfoPublisher()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
