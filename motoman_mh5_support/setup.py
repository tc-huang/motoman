from setuptools import find_packages, setup
from glob import glob
import os

package_name = 'motoman_mh5_support'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        (os.path.join('share', package_name, 'meshes/mh5/collision'), glob('meshes/mh5/collision/*')),
        (os.path.join('share', package_name, 'meshes/mh5/visual'), glob('meshes/mh5/visual/*')),
        (os.path.join('share', package_name, 'urdf'), glob('urdf/*')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='tc-huang',
    maintainer_email='tzu.chun.huang.tw@gmail.com',
    description='TODO: Package description',
    license='BSD-3-Clause',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'my_node = motoman_mh5_support.my_node:main'
        ],
    },
)
