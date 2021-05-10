/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
 
#ifndef BRAVE_IOS_BROWSER_API_HISTORY_BRAVE_HISTORY_API_H_
#define BRAVE_IOS_BROWSER_API_HISTORY_BRAVE_HISTORY_API_H_

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol HistoryServiceObserver;
@protocol HistoryServiceListener;

@class IOSHistoryNode;

NS_SWIFT_NAME(HistoryNode)
OBJC_EXPORT
@interface IOSHistoryNode : NSObject

@property(nonatomic, strong, readonly) NSURL* url;
@property(nonatomic, nullable, copy) NSString* title;
@property(nonatomic, nullable, copy) NSDate* dateAdded;

- (instancetype)initWithURL:(NSURL*)url
                      title:(NSString* _Nullable)title
                  dateAdded:(NSDate* _Nullable)dateAdded;
@end

NS_SWIFT_NAME(BraveHistoryAPI)
OBJC_EXPORT
@interface BraveHistoryAPI : NSObject

@property(class, readonly, getter = sharedHistoryAPI) BraveHistoryAPI* shared;
@property(nonatomic, readonly) bool isLoaded;

- (id<HistoryServiceListener>)addObserver:(id<HistoryServiceObserver>)observer;
- (void)removeObserver:(id<HistoryServiceListener>)observer;

- (void)addHistory:(IOSHistoryNode*)history;
- (void)removeHistory:(IOSHistoryNode*)history;
- (void)removeAllWithCompletion:(void(^)())completion;
- (void)searchWithQuery:(NSString* _Nullable)query maxCount:(NSUInteger)maxCount
                                       completion:(void(^)(NSArray<IOSHistoryNode*>* historyResults))completion;
@end

NS_ASSUME_NONNULL_END

#endif // BRAVE_IOS_BROWSER_API_HISTORY_BRAVE_HISTORY_API_H_