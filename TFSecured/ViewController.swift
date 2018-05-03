//
//  ViewController.swift
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

import UIKit


private let INPUT_NODE_NAME     = "input"
private let OUTPUT_NODE_NAME    = "output"
private let IMAGE_SIDE_SIZE     = 28


class ViewController: UIViewController {


    override func viewDidLoad() {
        super.viewDidLoad()
        
        
        DispatchQueue.global().async {
            let modelPath = Bundle.main.path(forResource: "saved_model-14-epoch", ofType: "pb")!
            let predictor = MNISTPredictor.initWith(modelPath,
                                                    inputNodeName: INPUT_NODE_NAME,
                                                    outputNodeName: OUTPUT_NODE_NAME)
            predictor.loadModel(nil)
            let inputImage =  #imageLiteral(resourceName: "nine").resize(targetSize: CGSize(width: IMAGE_SIDE_SIZE, height: IMAGE_SIDE_SIZE))
            let digit = predictor.predict(image: inputImage)
            print("Recognized Digit: \(digit)")
        }
    }
}

