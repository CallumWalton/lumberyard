/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <AzCore/Debug/TraceMessageBus.h>

namespace SandboxEditor
{
    /// Class to display errors during startup.
    /// This class stops handling errors once the regular UI is ready
    class StartupTraceHandler
        : public AZ::Debug::TraceMessageBus::Handler
    {
    public:
        StartupTraceHandler();
        ~StartupTraceHandler();

        //////////////////////////////////////////////////////////////////////
        // AZ::Debug::TraceMessageBus::Handler overrides
        //////////////////////////////////////////////////////////////////////
        bool OnPreAssert(const char* fileName, int line, const char* func, const char* message) override;
        bool OnException(const char* message) override;
        bool OnPreError(const char* window, const char* fileName, int line, const char* func, const char* message) override;
        bool OnPreWarning(const char* window, const char* fileName, int line, const char* func, const char* message) override;
        
        //////////////////////////////////////////////////////////////////////
        // Public interface
        //////////////////////////////////////////////////////////////////////
        //! Tells the trace handler to start collecting messages, instead of displaying them as they occur.
        //! Connects to the message bus to make sure collection can occur.
        void StartCollection();
        //! Ends collection, and displays all collected messages in one dialog.
        void EndCollectionAndShowCollectedErrors();
        //! @return if any errors occured during level load.
        bool HasAnyErrors() const { return m_errors.size() > 0; }

        //! Clears out any stored messages.
        void ClearMessages();

        //! Connects the trace handler to the trace message bus.
        void ConnectToMessageBus();
        //! Disconnects the trace handler from the trace message bus.
        void DisconnectFromMessageBus();

        //! @return if the trace handler is connected to the message bus.
        bool IsConnectedToMessageBus() const;

    private:

        /// The display behavior for messages.
        /// Some, like warnings, are only showed in a shared message dialog.
        /// Others, like Exceptions, are likely fatal and need to be displayed immediately. The program
        /// state after a fatal error may not be recoverable, so these messages can't be collected
        /// to be displayed later.
        enum class MessageDisplayBehavior
        {
            AlwaysShow,             ///< Always show this message, when collecting or not collecting messages.
            ShowWhenNotCollecting,  ///< Only show this message when collecting is not active, otherwise 
                                    ///< the collection system will show this later.
            OnlyCollect,            ///< Only collect this message, don't show it if not collecting.
        };

        void OnMessage(
            const char *message,
            AZStd::list<QString>* messageList,
            MessageDisplayBehavior messageDisplayBehavior);

        void ShowMessageBox(const QString& message);
        void ShowMessages(
            const AZStd::list<QString>& warnings,
            const AZStd::list<QString>& errors
            );

        mutable AZStd::recursive_mutex m_mutex; ///< Messages can come in from multiple threads, so use a mutex to lock things when necessary.

        AZStd::list<QString> m_errors; ///< The list of errors that occured while collecting.
        AZStd::list<QString> m_warnings; ///< The list of warnings that occured while collecting.
        bool m_isCollecting = false; ///< Tracks if the trace handler is collecting messages or displaying them as they occur.

        ///< The list of messages that occured on a thread that can't display a Qt popup.
        ///< These are intended for 1 at a time popups.
        AZStd::list<QString> m_mainThreadMessages;

    };
}