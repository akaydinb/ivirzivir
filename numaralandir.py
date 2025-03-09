#!/usr/bin/python

from os  import listdir
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
from PIL.ExifTags import TAGS


folder_dir = "."
sirano = 1

for images in listdir(folder_dir):
    if (images.endswith(".jpg")):
        print(images)

        img = Image.open(images);

        #https://jdhao.github.io/2019/07/31/image_rotation_exif_info/
        exif = img.getexif()
#        print(exif)
#        print(exif[274])
#        for k, v in exif.items():
#            print('{}: {}'.format(TAGS[k], v))

        # https://note.nkmk.me/en/python-pillow-rotate/
        if (exif[274] == 3):
            rotated = img.rotate(90, expand=True)
        elif (exif[274] == 6):
            rotated = img.rotate(270, expand=True)
        else:
            rotated = img

        #https://www.geeksforgeeks.org/adding-text-on-image-using-python-pil/
        draw = ImageDraw.Draw(rotated)
        font = ImageFont.truetype("NotoSansMono-Medium.ttf", 128)
        draw.text((25, 25), str(sirano).zfill(3), font=font, fill = (255, 0, 0));

        #img.show();
        #rotated.show();

        #input("Press Enter to continue...")
        #rotated.save( os.path.splitext(images)[0] + "_enum.jpg" );
        sirano = sirano + 1
