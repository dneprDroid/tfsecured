//
//  ViewController.swift
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

import UIKit


private let INPUT_NODE_NAME     = "nn_input"
private let OUTPUT_NODE_NAME    = "nn_output"


class ViewController: UIViewController {


    override func viewDidLoad() {
        super.viewDidLoad()
        
        recognize(image: #imageLiteral(resourceName: "digit"))
    }
    
    
    private func recognize(image: UIImage) {
        DispatchQueue.global().async {
            
            let modelPath = Bundle.main.path(forResource: "saved_model-encrypted", ofType: "pb")!
            let predictor = MNISTPredictor.initWith(modelPath,
                                                    inputNodeName: INPUT_NODE_NAME,
                                                    outputNodeName: OUTPUT_NODE_NAME)
            predictor.loadModel(key: "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM",
                                error: { _ in
                print("Loading proto file is failed.")
            })
            let inputImage =  image.resize(targetSize: CGSize(width:  MNIST_IMAGE_PIXEL_SIDE_SIZE,
                                                              height: MNIST_IMAGE_PIXEL_SIDE_SIZE))
            predictor.predict(image: inputImage,
                              success: { digit in
                print("Recognized Digit: \(digit)")
            }, error: {
                print("Recognition is failed!")
            })
            
        }
    }
}

