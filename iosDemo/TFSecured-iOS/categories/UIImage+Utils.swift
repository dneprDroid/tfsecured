//
//  UIImage+Utils.swift
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

import UIKit.UIImage

extension UIImage {
    
    func resize(targetSize newSize: CGFloat) -> UIImage {
        return self.resize(targetSize: CGSize(width: newSize, height: newSize))
    }
    
    func resize(targetSize newSize: CGSize) -> UIImage {
        let image = self
//        let size = image.size
        
//        let widthRatio  = targetSize.width  / size.width
//        let heightRatio = targetSize.height / size.height
        
//        var newSize: CGSize
//        if(widthRatio > heightRatio) {
//            newSize = CGSize(width: size.width * heightRatio, height:size.height * heightRatio)
//        } else {
//            newSize = CGSize(width: size.width * widthRatio,  height:size.height * widthRatio)
//        }
        
        let rect = CGRect(x:0, y:0, width: newSize.width, height:newSize.height)
        
        UIGraphicsBeginImageContextWithOptions(newSize, false, 1.0)
        image.draw(in: rect)
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage!
    }
}
