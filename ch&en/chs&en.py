import os
import pathlib
import tensorflow as tf
from keras.layers import *

AUTOTUNE = tf.data.experimental.AUTOTUNE

def data_handle(path):
    root_dir = os.getcwd() + os.sep + path
    root_dir_path = pathlib.Path(root_dir)

    all_image_filename = [str(jpg_path) for jpg_path in root_dir_path.glob('**/*.jpg')]
    all_image_label = [pathlib.Path(image_file).parent.name for image_file in all_image_filename]
    all_image_unique_labelname = list(set(all_image_label))

    name_index = dict((name, index) for index, name in enumerate(all_image_unique_labelname))
    image_lable_code = [name_index[pathlib.Path(path).parent.name] for path in all_image_filename]

    return all_image_filename, image_lable_code


# 2. 生成 dataset
# 借助文件名获取执行的图片，还有同时给出对应的标签信息
# 因为图片大小为256的数据跑得太慢，所以把图片缩放到32
def get_image_by_filename(filename, label):
    # 借助 tf 读入文件
    image_data = tf.io.read_file(filename)
    image_jpg = tf.image.decode_jpeg(image_data)
    image_resized = tf.image.resize(image_jpg, [32, 32])
    image_scale = image_resized / 255.0
    return image_scale, label


def only_get_image_by_filename(filename):
    # 借助 tf 读入文件
    image_info = []
    for f in filename:
        image_data = tf.io.read_file(f)
        image_jpg = tf.image.decode_jpeg(image_data)
        image_resized = tf.image.resize(image_jpg, [32, 32])
        image_scale = image_resized / 255.0
        image_info.append(image_scale)
    return image_info


# 3. 生成cnn网络，完成训练
class Model:
    def __init__(self):
        self.num_epochs = 10
        self.learning_rate = 0.001
        self.model = tf.keras.Sequential()


class LeNet5(Model):
    def __init__(self):
        super().__init__()

    def setModel(self):
        self.model.add(Conv2D(6, (5, 5), activation='relu', input_shape=(32, 32, 1)))
        self.model.add(MaxPooling2D(2, 2))
        self.model.add(Conv2D(16, (5, 5), activation='relu'))
        self.model.add(MaxPooling2D(2, 2))
        self.model.add(Flatten())
        self.model.add(Dense(120, activation='relu'))
        self.model.add(Dense(84, activation='relu'))
        self.model.add(Dense(65, activation='softmax'))

    def trainModel(self, train_data, tf_test_fea, tf_test_lab):
        self.model.summary()

        self.model.compile(
            optimizer=tf.keras.optimizers.Adam(learning_rate=self.learning_rate),
            loss=tf.keras.losses.sparse_categorical_crossentropy,
            metrics=['accuracy']
        )

        self.model.fit(train_data, epochs=self.num_epochs, validation_data=(tf_test_fea, tf_test_lab))


if __name__ == '__main__':
    # 1. 获取指定目录(子目录)下的所有的文件的名称、该文件所对应的标签名称（该文件的父目录的名称）==>> 编码 == list
    # 我把中英文的训练都放在了train_data文件夹下，同理中英文的测试都放在test_data文件夹下
    train_path = 'train_data'
    test_path = 'test_data'

    all_image_filename, train_image_lable_code = data_handle(train_path)
    all_test_image_filename, test_image_lable_code = data_handle(test_path)

    tf_train_feature_filenames = tf.constant(all_image_filename)
    tf_train_labels = tf.constant(train_image_lable_code)
    train_dataset = tf.data.Dataset.from_tensor_slices((tf_train_feature_filenames, tf_train_labels))

    tf_test_feature = tf.convert_to_tensor(only_get_image_by_filename(all_test_image_filename))
    tf_test_labels = tf.constant(test_image_lable_code)

    # map
    train_dataset = train_dataset.map(map_func=get_image_by_filename, num_parallel_calls=AUTOTUNE)

    train_dataset = train_dataset.shuffle(buffer_size=200000)
    train_dataset = train_dataset.batch(batch_size=32)
    train_dataset = train_dataset.prefetch(AUTOTUNE)

    lenet = LeNet5()
    lenet.setModel()
    lenet.trainModel(train_dataset, tf_test_feature, tf_test_labels)